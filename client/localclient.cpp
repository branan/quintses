#include "localclient.hpp"
#include "server/localserver.hpp"

LocalClient::LocalClient() {
  initializePlatform();
  m_server = new LocalServer();
  m_server->addClient(this);
  // TODO: launch local threads
}

LocalClient::~LocalClient() {
  finalizePlatform();
}


int LocalClient::mainloop() {
  return 0; // TODO: actually run some sort of game loop here
}

ServerIface* LocalClient::server() const {
  return m_server;
}