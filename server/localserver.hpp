#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <set>

#include <boost/thread.hpp>

#include "core/serveriface.hpp"

class LocalServer : public ServerIface {
public:
  LocalServer();

  void run();

  virtual void addClient(ClientIface*);
  virtual void makeClientPrivileged(ClientIface*);

  virtual int waitForTermination();

private:
  std::set<ClientIface*> m_clients;

  // thread management members
  bool m_running;
  boost::condition_variable m_status_cond;
  boost::mutex m_status_mutex;
  boost::shared_mutex m_clients_mutex; // read/write lock for clients list
};

#endif // QNT_SERVER_LOCALSERVER_H
