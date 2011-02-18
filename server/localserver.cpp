#include "localserver.hpp"
#include "core/clientiface.hpp"

#include "bullet/btphysics.hpp"

#include "core/messages/server/shutdownmsg.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"

namespace {
  struct ServerLauncher {
    void operator()(LocalServer* s) {
      s->run();
    }
  };
}

LocalServer::LocalServer() {
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

float matrix[16] = {
  1.f, 0.f, 0.f, 0.f,
  0.f, 1.f, 0.f, 0.f,
  0.f, 0.f, 1.f, 0.f,
  0.f, 0.f, 0.f, 1.f
};
void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  m_clients.insert(c);
  matrix[12]=-1.5f;
  matrix[14]=-8.f;
  ClientAddObjectParams ap;
  ClientTransObjectParams tp;
  ap.m_template = "Triangle";
  tp.m_transform = matrix;
  ap.m_objid = tp.m_objid = 1;
  c->pushMessage(new ClientAddDrawableMsg(ap));
  c->pushMessage(new ClientTransDrawableMsg(tp));
  matrix[12]=1.5f;
  matrix[14]=-3.f;
  ap.m_objid = tp.m_objid = 2;
  c->pushMessage(new ClientAddDrawableMsg(ap));
  c->pushMessage(new ClientTransDrawableMsg(tp));
}

void LocalServer::removeClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  m_clients.erase(c);
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
