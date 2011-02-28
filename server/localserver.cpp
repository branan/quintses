#include "localserver.hpp"
#include "core/clientiface.hpp"

#include "bullet/btphysics.hpp"

#include "core/messages/server/inputmsg.hpp"
#include "core/messages/server/shutdownmsg.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"
#include "core/messages/client/clientdelobjectmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"
#include "core/util/atomic.hpp"

namespace {
  struct ServerLauncher {
    void operator()(LocalServer* s) {
      s->run();
    }
  };

  float matrix[16] = {
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
  };
}

LocalServer::LocalServer() : m_next_id(1) {
  m_running = false;
  ServerLauncher launcher;
  boost::thread t(launcher, this);
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
  // server thread is on its own at this point
}

void LocalServer::run() {
  m_physics = new BtPhysics(this);
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  matrix[14] = -30.f;
  m_physics->addPhysical(getNextIdentifier(), "Floor", matrix);
  matrix[12] = -13.f;
  matrix[14] = 0.f;
  m_physics->addPhysical(getNextIdentifier(), "LWall", matrix);
  matrix[12] = 13.f;
  m_physics->addPhysical(getNextIdentifier(), "RWall", matrix);

  // Set the matrix to the default client position (HAX)
  matrix[12] = 0.f;
  matrix[13] = -12.f;
  matrix[14] = -25.f;
  bool looping = true;
  while(looping) {
    ServerMsg* msg;
    msg = m_msg_queue.wait_pop();
    switch(msg->type()) {
      case ServerMsg::ShutdownMessage:
        looping = false;
        delete msg;
        break;
      case ServerMsg::InputMessage:
        m_physics->pushMessage(msg);
        break;
      default:
        delete msg;
        break;
    }
  }

  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    i->first->serverClosed();
  }
  m_physics->finish();
  delete m_physics;
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  ClientInfo ci;
  ci.m_objid = getNextIdentifier();

  // Add player drawables to all clients
  // locations will be synched on next physics tick
  ClientAddObjectParams ap_new("Player", ci.m_objid);
  c->pushMessage(new ClientAddDrawableMsg(ap_new));
  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    ClientAddObjectParams ap_old("Player", i->second.m_objid);
    c->pushMessage(new ClientAddDrawableMsg(ap_old));
    i->first->pushMessage(new ClientAddDrawableMsg(ap_new));
  }
  m_clients.insert(std::make_pair(c, ci));
  m_physics->addPhysical(ci.m_objid, "Player", matrix, true);
}

void LocalServer::removeClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  uint32_t client_id = m_clients[c].m_objid;
  m_clients.erase(c);
  m_physics->delPhysical(client_id, true);
  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    i->first->pushMessage(new ClientDelDrawableMsg(client_id));
  }
}

// TODO: privileged clients. For now any client can make any call
void LocalServer::makeClientPrivileged(ClientIface* ) { }

bool LocalServer::isLocal() const {
  return true;
}

void LocalServer::pushMessage(ServerMsg* msg) {
  m_msg_queue.push(msg);
}

int LocalServer::waitForTermination() const {
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
  return 0;
}

void LocalServer::pushClientTransforms(LocalServer::ClientTransList& trans) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  for(auto i = trans.begin(); i != trans.end(); ++i) {
    ClientTransObjectParams tp;
    tp.m_transform = i->second;
    tp.m_objid = i->first;
    for(auto j = m_clients.begin(); j != m_clients.end(); ++j)
      j->first->pushMessage(new ClientTransDrawableMsg(tp));
    delete[] i->second;
  }
}

uint32_t LocalServer::getNextIdentifier() {
  return fetchAndIncrement(m_next_id);
}
