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

RemoteServer::RemoteServer(const char* srv) : m_srvname(srv), m_client(0), m_socket(m_service), m_wrapper(m_socket) {
  if(!srv)
    m_srvname = "localhost";
}

RemoteServer::~RemoteServer() {}


void RemoteServer::addClient(ClientIface* cli) {
  m_client = cli;
  boost::asio::ip::tcp::resolver resolver(m_service);
  boost::asio::ip::tcp::resolver::query query(m_srvname, "55555");
  boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
  m_socket.connect(*iterator);
  if(!m_socket.is_open()) {
    std::cerr << "Failed to connect to " << m_srvname << std::endl;
    return;
  }
  
  m_running = false;
  ServerLauncher launcher;
  boost::thread t(launcher, this);
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
  
  boost::asio::async_read(m_socket, boost::asio::buffer((char*)&m_next_msg_type, 4),
                          boost::bind(&RemoteServer::readMsg, this, boost::asio::placeholders::error));
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
  
  m_service.post(boost::bind(&RemoteServer::writeMsg, this, msg));
}

int RemoteServer::waitForTermination() const {
  m_service.stop();
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

  m_service.run();
  if(m_client)
    m_client->serverClosed();

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void RemoteServer::writeMsg(ServerMsg *msg) {
  try {
    m_wrapper.writePod<uint32_t>(msg->type());
    msg->write(m_wrapper);
  } catch (boost::system::system_error& e) {
    switch(e.code().value()) {
      case ENOENT:
        std::cerr << "QntClient: server socket closed.\n";
        break;
      default:
        std::cerr << "QntClient: got error reading from socket <" << strerror(e.code().value()) << ">\n";
    }
    m_service.stop();
  }
  delete msg;
}

void RemoteServer::readMsg(const boost::system::error_code &err) {
  if(err) {
    switch(err.value()) {
      case ENOENT:
        std::cerr << "QntClient: server socket closed.\n";
        break;
      default:
        std::cerr << "QntClient: got error reading from socket <" << strerror(err.value()) << ">\n";
    }
    m_service.stop();
    return;
  }
  ClientMsg *msg = ClientMsg::create(m_next_msg_type);
  if(msg) {
    msg->read(m_wrapper);
    if(m_client)
      m_client->pushMessage(msg);
    else
      delete msg;
    boost::asio::async_read(m_socket, boost::asio::buffer((char*)&m_next_msg_type, 4),
                            boost::bind(&RemoteServer::readMsg, this, boost::asio::placeholders::error));
  } else {
    std::cerr << "QntClient: received unknown message of type <" << m_next_msg_type << ">\n";
    std::cerr << "QntClient: closing stream due to unknown error.\n";
    m_service.stop();
  }
}
