#include "localclient.hpp"
#include "opengl/glloader.hpp"
#include "server/localserver.hpp"

LocalClient::LocalClient() : m_finished(false) {
  initializePlatform();
  m_server = new LocalServer();
  m_server->addClient(this);
  m_server->makeClientPrivileged(this);
  m_loader = new GlLoader(m_platform);
  // TODO: launch audio thread
}

LocalClient::~LocalClient() {
  m_loader->finish();
  //TODO: end audio thread
  finalizePlatform();
}


int LocalClient::mainloop() {
  while(!m_finished) {
    platformEvents();
  }
  if(m_server->isLocal()) {
    m_server->shutdown();
    m_server->waitForTermination();
  }
  return 0;
}

ServerIface* LocalClient::server() const {
  return m_server;
}
