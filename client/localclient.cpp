#include "localclient.hpp"
#include "server/localserver.hpp"

LocalClient::LocalClient() : m_finished(false) {
  initializePlatform();
  m_server = new LocalServer();
  m_server->addClient(this);
  // TODO: launch local threads
}

LocalClient::~LocalClient() {
  finalizePlatform();
}


int LocalClient::mainloop() {
  while(!m_finished) {
    platformEvents();
  }
  return 0;
}

ServerIface* LocalClient::server() const {
  return m_server;
}