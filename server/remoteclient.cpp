#pragma GCC diagnostic ignored "-Wdeprecated"

#include "remoteclient.hpp"
#include "core/messages/client/clientmsg.hpp"
#include "core/messages/server/servermsg.hpp"
#include "core/serveriface.hpp"

#include <iostream>

RemoteClient::RemoteClient(ServerIface* srv, boost::asio::io_service &service)
  : m_server(srv), m_service(service), m_socket(service), m_wrapper(m_socket), m_marked_for_delete(false) {}

RemoteClient::~RemoteClient() {}

ServerIface* RemoteClient::server() const {
  return m_server;
}

void RemoteClient::pushMessage(ClientMsg* msg) {
  m_service.post(boost::bind(&RemoteClient::writeMsg, this, msg));
}

void RemoteClient::spinup() {
  m_server->addClient(this);
  boost::asio::async_read(m_socket, boost::asio::buffer((char*)&m_next_msg_type, 4),
                          boost::bind(&RemoteClient::readMsg, this, boost::asio::placeholders::error));
}

void RemoteClient::writeMsg(ClientMsg *msg) {
  try {
    m_wrapper.writePod<uint32_t>(msg->type());
    msg->write(m_wrapper);
  } catch(boost::system::system_error& e) {
    if(!m_marked_for_delete) {
      switch(e.code().value()) {
        case ENOENT:
          std::cerr << "QntServer: client socket closed.\n";
          break;
        default:
          std::cerr << "QntServer: got error reading from socket <" << strerror(e.code().value()) << ">\n";
      }
      m_server->removeClient(this);
      m_marked_for_delete = true;
      m_service.post(boost::bind(&RemoteClient::deleteSelf, this));
    }
  }
  delete msg;
  // we delete ourselves asynchronously to make sure we're in the clear.
}

void RemoteClient::readMsg(const boost::system::error_code& err) {
  if(m_marked_for_delete) {
    return;
  } else if(err) {
    switch(err.value()) {
      case ENOENT:
        std::cerr << "QntServer: client socket closed.\n";
        break;
      default:
        std::cerr << "QntServer: got error reading from socket <" << strerror(err.value()) << ">\n";
    }
    m_server->removeClient(this);
    m_marked_for_delete = true;
    m_service.post(boost::bind(&RemoteClient::deleteSelf, this));
    return;
  }
  ServerMsg *msg = ServerMsg::create(m_next_msg_type, this);
  if(msg) {
    msg->read(m_wrapper);
    m_server->pushMessage(msg);
    boost::asio::async_read(m_socket, boost::asio::buffer((char*)&m_next_msg_type, 4),
                            boost::bind(&RemoteClient::readMsg, this, boost::asio::placeholders::error));
  } else {
    std::cerr << "QntServer: received unknown message of type <" << m_next_msg_type << ">\n";
    std::cerr << "QntServer: closing stream due to unknown error.\n";
    m_server->removeClient(this);
    m_marked_for_delete = true;
    m_service.post(boost::bind(&RemoteClient::deleteSelf, this));
  }
}

void RemoteClient::deleteSelf() {
  delete this;
}
