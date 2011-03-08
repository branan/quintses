#ifndef QNT_SERVER_REMOTECLIENT_HPP
#define QNT_SERVER_REMOTECLIENT_HPP

#include "core/clientiface.hpp"
#include "core/util/queue.hpp"
#include "core/util/socket.hpp"

/// \brief Networked client
/// \ingroup Server
/// \ingroup Client
/// \ingroup Network
/// This class implements a socket interface to a client.
class RemoteClient : public ClientIface {
public:
  RemoteClient(ServerIface*, boost::asio::io_service&);
  virtual ~RemoteClient();

  virtual ServerIface* server() const;
  virtual void pushMessage(ClientMsg*);
  virtual void serverClosed() { m_socket.close(); }

  /// \brief Get the internal socket stream
  boost::asio::ip::tcp::socket& socket() { return m_socket; }

  /// \brief Add client to server and start reading messages
  void spinup();

private:
  ServerIface* m_server;
  boost::asio::io_service& m_service;
  boost::asio::ip::tcp::socket m_socket;
  AsioSocket<boost::asio::ip::tcp> m_wrapper;

  void writeMsg(ClientMsg*);
  void readMsg(const boost::system::error_code&);
  void deleteSelf();
  uint32_t m_next_msg_type;
  bool m_marked_for_delete;
};

#endif // QNT_SERVER_REMOTECLIENT_HPP
