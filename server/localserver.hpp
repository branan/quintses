#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <map>

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
  struct matrix {
    float mat[16];
  };
  std::map<ClientIface*, uint32_t> m_clients;
  std::map<uint32_t, matrix> m_matrices;
  uint32_t m_next_id; // this is the global identifier counter. Hopefully we never have more than ~4 Billion objects

  // event queue
  queue<ServerMsg*> m_msg_queue;

  // worker threads/subsystems
  PhysicsIface* m_physics;

  // thread management members
  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
  boost::shared_mutex m_clients_mutex; // read/write lock for clients list

  uint32_t getNextIdentifier();
};

#endif // QNT_SERVER_LOCALSERVER_H
