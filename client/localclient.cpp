#include "localclient.hpp"
#include "server/localserver.hpp"
#include "core/loaderiface.hpp"
#include "core/renderiface.hpp"
#include "core/messages/client/clientmsg.hpp"
#include "core/messages/server/shutdownmsg.hpp"

#include "openal/alaudio.hpp"

LocalClient::LocalClient() : m_finished(false) {
  initializePlatform();
  m_audio = new AlAudio;
  m_server = new LocalServer();
  m_server->addClient(this);
  m_server->makeClientPrivileged(this);
}

LocalClient::~LocalClient() {
  finalizePlatform();
  m_audio->finish();
  delete m_audio;
}


int LocalClient::mainloop() {
  while(!m_finished) {
    ClientMsg* msg;
    while(m_msg_queue.try_pop(msg)) {
      switch(msg->type()) {
        case ClientMsg::AddDrawable:
        case ClientMsg::TransDrawable:
          m_renderer->pushMessage(msg);
          break;
        default:
          break;
      }
      delete msg;
    }
    platformEvents();
    m_renderer->draw();
    platformSwapBuffer();
  }
  if(m_server->isLocal()) {
    m_server->pushMessage(new ServerShutdownMsg(this));
    m_server->waitForTermination();
  }
  delete m_server;
  return 0;
}

ServerIface* LocalClient::server() const {
  return m_server;
}

void LocalClient::pushMessage(ClientMsg* msg) {
  m_msg_queue.push(msg);
}
