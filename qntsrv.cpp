#include "server/localserver.hpp"
#include "server/remoteclient.hpp"

#include <boost/asio.hpp>
using boost::asio::ip::tcp;

struct WatcherThread {
  WatcherThread(boost::asio::io_service &svc, LocalServer *srv)
    : service(svc), server(srv)
  {}
  void operator()() {
    server->waitForTermination();
    service.stop();
  }
  boost::asio::io_service &service;
  LocalServer *server;
};

class ListenServer {
public:
  ListenServer(boost::asio::io_service &service, tcp::endpoint &endpoint, LocalServer *srv) 
    : m_service(service), m_acceptor(service, endpoint), m_server(srv) {
    m_next_client = new RemoteClient(m_server);
    m_acceptor.async_accept(*m_next_client->stream().rdbuf(), boost::bind(&ListenServer::handleAccept, this, boost::asio::placeholders::error));
  }

  void handleAccept(const boost::system::error_code &error) {
    if(!error) {
      m_next_client->spinup();
      m_next_client = new RemoteClient(m_server);
      m_acceptor.async_accept(*m_next_client->stream().rdbuf(), boost::bind(&ListenServer::handleAccept, this, boost::asio::placeholders::error));
    }
  }
private:
  boost::asio::io_service &m_service;
  tcp::acceptor m_acceptor;
  LocalServer *m_server;
  RemoteClient *m_next_client;
};

int main(int, char**) {
  boost::asio::io_service io_service;
  LocalServer *s = new LocalServer;
  tcp::endpoint endpoint(tcp::v4(), 55555);
  ListenServer l(io_service, endpoint, s);
  boost::thread watcher(WatcherThread(io_service, s));
  io_service.run();
  delete s;
}
