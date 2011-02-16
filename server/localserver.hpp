#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <set>

#include <boost/thread.hpp>

#include "core/serveriface.hpp"
#include "core/util/queue.hpp"

class PhysicsIface;

class LocalServer : public ServerIface {
public:
  LocalServer();

  void run();

  virtual void addClient(ClientIface*);
  virtual void removeClient(ClientIface*);
  virtual void makeClientPrivileged(ClientIface*);
  virtual bool isLocal() const;

  virtual void pushMessage(ServerMsg*);

  virtual int waitForTermination() const;

private:
  std::set<ClientIface*> m_clients;

  // event queue
  queue<ServerMsg*> m_msg_queue;

  // worker threads/subsystems
  PhysicsIface* m_physics;

  // thread management members
  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
  boost::shared_mutex m_clients_mutex; // read/write lock for clients list
};

#endif // QNT_SERVER_LOCALSERVER_H
