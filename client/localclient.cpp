#include "localclient.hpp"
#include "server/localserver.hpp"
#include "core/loaderiface.hpp"
#include "core/messages/server/shutdownmsg.hpp"

#include "openal/alaudio.hpp"

LocalClient::LocalClient() : m_finished(false) {
  initializePlatform();
  m_server = new LocalServer();
  m_server->addClient(this);
  m_server->makeClientPrivileged(this);
  m_audio = new AlAudio;
}

LocalClient::~LocalClient() {
  m_audio->finish();
  delete m_audio;
  finalizePlatform();
}


int LocalClient::mainloop() {
  while(!m_finished) {
    platformEvents();
  }
  if(m_server->isLocal()) {
    m_server->pushMessage(new ServerShutdownMsg(this));
    m_server->waitForTermination();
  }
  return 0;
}

ServerIface* LocalClient::server() const {
  return m_server;
}
