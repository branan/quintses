#include "localserver.hpp"

#include "core/messages/server/shutdownmsg.hpp"

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
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  for(;;) {
    ServerMessage* msg;
    {
      boost::mutex::scoped_lock lock(m_queue_mutex);
      while(m_msg_queue.empty())
        m_queue_cond.wait(lock);
      msg = m_msg_queue.front();
      m_msg_queue.pop_front();
    }
    if(msg->type() == ServerMessage::ShutdownMessage)
      break;
  }

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  m_clients.insert(c);
}

// TODO: privileged clients. For now any client can make any call
void LocalServer::makeClientPrivileged(ClientIface* ) { }

bool LocalServer::isLocal() const {
  return true;
}

void LocalServer::pushMessage(ServerMessage* msg) {
  boost::mutex::scoped_lock lock(m_queue_mutex);
  bool was_empty = m_msg_queue.empty();
  m_msg_queue.push_back(msg);
  lock.unlock();
  if(was_empty)
    m_queue_cond.notify_one();
}

void LocalServer::shutdown(ClientIface*){
  //TODO: Check if client is priveleged
  pushMessage(new ServerShutdownMsg);
}

int LocalServer::waitForTermination() const {
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
  return 0;
}
