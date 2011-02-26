#include "localclient.hpp"
#include "core/loaderiface.hpp"
#include "core/renderiface.hpp"
#include "core/serveriface.hpp"
#include "core/messages/client/clientmsg.hpp"
#include "core/messages/server/shutdownmsg.hpp"

#include "openal/alaudio.hpp"

LocalClient::LocalClient(ServerIface *srv) : m_server(srv), m_finished(false) {
  initializePlatform();
  m_audio = new AlAudio;
  m_server->addClient(this);
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
        case ClientMsg::DelDrawable:
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
  }
  m_server->waitForTermination(); // on a remote server this will close the socket
  delete m_server;
  return 0;
}

ServerIface* LocalClient::server() const {
  return m_server;
}

void LocalClient::pushMessage(ClientMsg* msg) {
  m_msg_queue.push(msg);
}
