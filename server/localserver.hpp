#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <map>
#include <vector>
#include <set>

#include <boost/thread.hpp>

#include "core/serveriface.hpp"
#include "core/util/queue.hpp"

class PhysicsIface;
class ServerLoadPlayerMsg;

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

  typedef std::vector<std::pair<uint32_t,float*> > ClientTransList;
  inline uint32_t getClientId(ClientIface* cli) { boost::shared_lock<boost::shared_mutex> lock(m_clients_mutex); return m_clients[cli]; }
  void pushClientTransforms(ClientTransList&);

private:
  std::map<ClientIface*, uint32_t> m_clients;
  std::set<ClientIface*> m_lobby_clients;
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

  void loadPlayer(ServerLoadPlayerMsg*);
};

#endif // QNT_SERVER_LOCALSERVER_H
