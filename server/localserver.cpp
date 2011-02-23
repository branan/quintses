#include "localserver.hpp"
#include "core/clientiface.hpp"

#include "bullet/btphysics.hpp"

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
  m_physics = new BtPhysics();
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
        break;
    }
    delete msg;
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
  0.f, 0.f, 0.f, 1.f
};
void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  uint32_t client_id = getNextIdentifier();
  matrix m;
  memcpy(m.mat, default_matrix, 64);
  m.mat[12]=-4.f + 2.f * client_id;
  m.mat[14]=-8.f;

  // send the new client all the existing objects
  ClientAddObjectParams ap("Player");
  ClientTransObjectParams tp;
  for(std::map<uint32_t, matrix>::iterator i = m_matrices.begin(); i != m_matrices.end(); ++i) {
    tp.m_transform = i->second.mat;
    ap.m_objid = tp.m_objid = i->first;
    c->pushMessage(new ClientAddDrawableMsg(ap));
    c->pushMessage(new ClientTransDrawableMsg(tp));
  }
  m_matrices.insert(std::make_pair(client_id, m));
  m_clients.insert(std::make_pair(c, client_id));

  // send every client (including the new one) the new object
  tp.m_transform = m.mat;
  for(std::map<ClientIface*, uint32_t>::iterator i = m_clients.begin(); i != m_clients.end(); ++i) {
    ap.m_objid = tp.m_objid = client_id;
    i->first->pushMessage(new ClientAddDrawableMsg(ap));
    i->first->pushMessage(new ClientTransDrawableMsg(tp));
  }
}

void LocalServer::removeClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  uint32_t client_id = m_clients[c];
  m_clients.erase(c);
  m_matrices.erase(client_id);
  for(std::map<ClientIface*, uint32_t>::iterator i = m_clients.begin(); i != m_clients.end(); ++i) {
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
