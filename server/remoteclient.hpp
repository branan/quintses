#ifndef QNT_SERVER_REMOTECLIENT_HPP
#define QNT_SERVER_REMOTECLIENT_HPP

#include "core/clientiface.hpp"
#include "core/util/queue.hpp"
#include <boost/asio.hpp>

class RemoteClient : public ClientIface {
public:
  RemoteClient(ServerIface*);
  virtual ~RemoteClient();

  virtual ServerIface* server() const;
  virtual void pushMessage(ClientMsg*);
  virtual void serverClosed() { m_stream.close(); }

  void spinup();
  void run();
  boost::asio::ip::tcp::iostream& stream() { return m_stream; }

private:
  ServerIface* m_server;
  boost::asio::ip::tcp::iostream m_stream;
};

#endif // QNT_SERVER_REMOTECLIENT_HPP
