#ifndef QNT_SERVER_BULLET_BYPHYSICS_HPP
#define QNT_SERVER_BULLET_BYPHYSICS_HPP

#include <boost/thread.hpp>

#include "core/physicsiface.hpp"
#include "core/util/queue.hpp"

#include <map>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;

class LocalServer;

struct PhysicsObject;

class BtPhysics : public PhysicsIface {
public:
  BtPhysics(LocalServer*);
  virtual ~BtPhysics();

  virtual void pushMessage(ServerMsg*);
  virtual void finish();

  virtual void addPlayerPhysical(uint32_t);
  virtual void delPlayerPhysical(uint32_t);

  void run();
private:
  std::map<uint32_t, PhysicsObject*> m_objects;
  queue<ServerMsg*> m_msg_queue;
  LocalServer* m_server;

  // thread management variables
  bool m_running, m_finish;
  boost::mutex m_status_mutex;
  boost::condition_variable m_status_cond;
  
  // bullet world classes
  btBroadphaseInterface* m_broadphase;
  btCollisionDispatcher* m_dispatcher;
  btDefaultCollisionConfiguration* m_collision_config;
  btDiscreteDynamicsWorld* m_world;
  btSequentialImpulseConstraintSolver* m_solver;
};

#endif // QNT_SERVER_BULLET_BYPHYSICS_HPP
