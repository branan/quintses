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

  m_physics->finish();
  delete m_physics;
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

float default_matrix[16] = {
  1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, -8.f, 1.f
};
void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  ClientInfo ci;
  ci.m_objid = getNextIdentifier();
  memcpy(ci.m_matrix, default_matrix, 64);

  // Synchronize player drawables across all clients
  ClientAddObjectParams ap_new("Player", ci.m_objid);
  ClientTransObjectParams tp_new(ci.m_matrix, ci.m_objid);
  c->pushMessage(new ClientAddDrawableMsg(ap_new));
  c->pushMessage(new ClientTransDrawableMsg(tp_new));
  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    ClientAddObjectParams ap_old("Player", i->second.m_objid);
    ClientTransObjectParams tp_old(i->second.m_matrix, i->second.m_objid);
    c->pushMessage(new ClientAddDrawableMsg(ap_old));
    c->pushMessage(new ClientTransDrawableMsg(tp_old));
    i->first->pushMessage(new ClientAddDrawableMsg(ap_new));
    i->first->pushMessage(new ClientTransDrawableMsg(tp_new));
  }
  m_clients.insert(std::make_pair(c, ci));
  m_physics->addPlayerPhysical(ci.m_objid);
}

void LocalServer::removeClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  uint32_t client_id = m_clients[c].m_objid;
  m_clients.erase(c);
  m_physics->delPlayerPhysical(client_id);
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

uint32_t LocalServer::getNextIdentifier() {
  return fetchAndIncrement(m_next_id);
}
