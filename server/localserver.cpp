#include "localserver.hpp"

namespace {
  struct ServerLauncher {
    void operator()(LocalServer* s) {
      s->run();
    }
  };
}

LocalServer::LocalServer()
  : m_running(true)
{
  //TODO: add some sort of initialization lock
  ServerLauncher launcher;
  boost::thread t(launcher, this);
  // server thread is on its own at this point
}

void LocalServer::run() {
  {
    boost::lock_guard<boost::mutex> lock(m_term_mutex);
    m_running = false;
  }
  m_term_cond.notify_all();
}

void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  m_clients.insert(c);
}

// TODO: privileged clients. For now any client can make any call
void LocalServer::makeClientPrivileged(ClientIface* ) { }

int LocalServer::waitForTermination() {
  boost::unique_lock<boost::mutex> lock(m_term_mutex);
  while(m_running) {
    m_term_cond.wait(lock);
  }
  return 0;
}
