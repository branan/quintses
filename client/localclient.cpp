#include "localclient.hpp"
#include "server/localserver.hpp"

LocalClient::LocalClient() {
  m_server = new LocalServer();
  m_server->addClient(this);
  // TODO: launch local threads
}

int LocalClient::mainloop() {
  return 0; // TODO: actually run some sort of game loop here
}

ServerIface* LocalClient::server() const {
  return m_server;
}