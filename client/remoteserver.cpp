#include "remoteserver.hpp"
#include "core/messages/server/servermsg.hpp"
#include "core/messages/client/clientmsg.hpp"

#include <boost/foreach.hpp>
#include <core/clientiface.hpp>

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
  unsigned int type = msg->type();
  m_stream.write((char*)&type, 4);
  msg->write(m_stream);
}

int RemoteServer::waitForTermination() const {
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
    unsigned int type;
    m_stream.read((char*)&type, 4);
    ClientMsg *msg = ClientMsg::create(type);
    msg->read(m_stream);
    if(m_client)
      m_client->pushMessage(msg);
    else
      delete msg;
  }
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

