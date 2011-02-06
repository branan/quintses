#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <deque>
#include <set>

#include <boost/thread.hpp>

#include "core/serveriface.hpp"

class PhysicsIface;

class LocalServer : public ServerIface {
public:
  LocalServer();

  void run();

  virtual void addClient(ClientIface*);
  virtual void makeClientPrivileged(ClientIface*);
  virtual bool isLocal() const;

  virtual void pushMessage(ServerMessage*);

  virtual int waitForTermination() const;

private:
  std::set<ClientIface*> m_clients;

  // event queue
  std::deque<ServerMessage*> m_msg_queue;
  boost::mutex m_queue_mutex;
  boost::condition_variable m_queue_cond;

  // worker threads/subsystems
  PhysicsIface* m_physics;

  // thread management members
  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
  boost::shared_mutex m_clients_mutex; // read/write lock for clients list
};

#endif // QNT_SERVER_LOCALSERVER_H
