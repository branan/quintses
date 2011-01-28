#include "localserver.hpp"

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
  boost::unique_lock<boost::mutex> lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
  // server thread is on its own at this point
}

void LocalServer::run() {
  {
    boost::lock_guard<boost::mutex> lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  boost::xtime xt;
  boost::xtime_get(&xt, boost::TIME_UTC);
  xt.sec += 1;
  boost::thread::sleep(xt);

  {
    boost::lock_guard<boost::mutex> lock(m_status_mutex);
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

int LocalServer::waitForTermination() {
  boost::unique_lock<boost::mutex> lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
  return 0;
}
