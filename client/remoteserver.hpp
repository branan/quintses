#ifndef QNT_CLIENT_REMOTESERVER_HPP
#define QNT_CLIENT_REMOTESERVER_HPP

#include "core/serveriface.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>

class RemoteServer : public ServerIface {
public:
  RemoteServer(const char*);
  virtual ~RemoteServer();

  // client management
  virtual void addClient(ClientIface*);
  virtual void removeClient(ClientIface*);
  virtual void makeClientPrivileged(ClientIface*);
  virtual bool isLocal() const;
  
  // Server message interface
  virtual void pushMessage(ServerMsg*);
  
  // General management
  virtual int waitForTermination() const;

  void run();

private:
  ClientIface *m_client;
  mutable boost::asio::ip::tcp::iostream m_stream;

  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
};

#endif // QNT_CLIENT_REMOTESERVER_HPP
