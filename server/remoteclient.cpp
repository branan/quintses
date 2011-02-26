#pragma GCC diagnostic ignored "-Wdeprecated"

#include "remoteclient.hpp"
#include "core/messages/client/clientmsg.hpp"
#include "core/messages/server/servermsg.hpp"
#include "core/serveriface.hpp"

#include <iostream>

namespace {
  struct ClientLauncher {
    void operator()(RemoteClient* c) {
      c->run();
    }
  };
}

RemoteClient::RemoteClient(ServerIface* srv)
  : m_server(srv) {}

RemoteClient::~RemoteClient() {}

ServerIface* RemoteClient::server() const {
  return m_server;
}

void RemoteClient::pushMessage(ClientMsg* msg) {
  uint32_t type = msg->type();
  m_stream.write((char*)&type, 4);
  msg->write(m_stream);
  m_stream.flush();
  delete msg;
}

void RemoteClient::spinup() {
  ClientLauncher launcher;
  boost::thread t(launcher, this);
}

void RemoteClient::run() {
  m_server->addClient(this);
  while(m_stream.good()) {
    uint32_t type;
    m_stream.read((char*)&type, 4);
    // check good again, in case read failed due to disconnect
    if(m_stream.good()) {
      ServerMsg *msg = ServerMsg::create(type, this);
      if(msg) {
        msg->read(m_stream);
        msg->m_sender = this;
        m_server->pushMessage(msg);
      } else {
        std::cerr << "QntServer: received unknown mesage of type <" << type << ">\n";
        std::cerr << "QntServer: closing socket after unknown error\n";
        m_stream.close();
      }
    }
  }
  m_server->removeClient(this);
  delete this;
}
