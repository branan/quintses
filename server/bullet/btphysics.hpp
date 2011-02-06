#ifndef QNT_SERVER_BULLET_BYPHYSICS_HPP
#define QNT_SERVER_BULLET_BYPHYSICS_HPP

#include <boost/thread.hpp>

#include "core/physicsiface.hpp"

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btSequentialImpulseConstraintSolver;

class BtPhysics : public PhysicsIface {
public:
  BtPhysics();
  virtual ~BtPhysics();

  virtual void finish();

  void run();
private:
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
