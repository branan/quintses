#ifndef QNT_CLIENT_REMOTESERVER_HPP
#define QNT_CLIENT_REMOTESERVER_HPP

#include "core/serveriface.hpp"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <set>

/// \brief A server running in a different process
/// \ingroup Client
/// \ingroup Server
/// \ingroup Network
/// This class wraps a server that is running elsewhere.
/// It forwards messages across a network socket to the
/// appropriate server, and dispatches any received messages
/// to the local client.
class RemoteServer : public ServerIface {
public:
  /// \brief Open a connection to a remote server
  /// \param host The hostname of the remote server
  RemoteServer(const char* host);
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

  /// \brief Main thread function
  /// This function is the mainloop for the RemoteServer's thread
  void run();

private:
  ClientIface *m_client;
  mutable boost::asio::ip::tcp::iostream m_stream;

  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
};

#endif // QNT_CLIENT_REMOTESERVER_HPP
