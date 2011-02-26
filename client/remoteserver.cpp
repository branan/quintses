#include "remoteserver.hpp"
#include "core/messages/server/servermsg.hpp"
#include "core/messages/client/clientmsg.hpp"
#include "core/clientiface.hpp"

#include <boost/foreach.hpp>

#include <iostream>

namespace {
  struct ServerLauncher {
    void operator()(RemoteServer* s) {
      s->run();
    }
  };
}

RemoteServer::RemoteServer(const char* srv) {}

RemoteServer::~RemoteServer() {}


void RemoteServer::addClient(ClientIface* cli) {
  m_client = cli;
  m_stream.connect("localhost", "55555");
  m_running = false;
  ServerLauncher launcher;
  boost::thread t(launcher, this);
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
}

void RemoteServer::removeClient(ClientIface* cli) {
  if(cli == m_client)
    m_client = 0;
}

void RemoteServer::makeClientPrivileged(ClientIface*) {}

bool RemoteServer::isLocal() const {
  return false;
}

void RemoteServer::pushMessage(ServerMsg* msg) {
  if(!m_running)
    return;
  uint32_t type = msg->type();
  m_stream.write((char*)&type, 4);
  msg->write(m_stream);
}

int RemoteServer::waitForTermination() const {
  m_stream.close();
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
  return 0;
}

void RemoteServer::run() {
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();
  while(m_stream) {
    uint32_t type;
    m_stream.read((char*)&type, 4);
    if(m_stream.good()) {
      ClientMsg *msg = ClientMsg::create(type);
      if(msg) {
        msg->read(m_stream);
        if(m_client)
          m_client->pushMessage(msg);
        else
          delete msg;
      } else {
        std::cerr << "QntClient: received unknown message of type <" << type << ">\n";
        std::cerr << "QntClient: closing stream due to unknown error.\n";
        m_stream.close();
      }
    }
  }
  m_client->serverClosed();
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

