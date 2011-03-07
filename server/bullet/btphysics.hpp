#ifndef QNT_SERVER_BULLET_BYPHYSICS_HPP
#define QNT_SERVER_BULLET_BYPHYSICS_HPP

#include <boost/thread.hpp>

#include "core/physicsiface.hpp"
#include "core/util/queue.hpp"

#include <map>
#include <stdint.h>

class btBroadphaseInterface;
class btCollisionDispatcher;
class btDefaultCollisionConfiguration;
class btDiscreteDynamicsWorld;
class btGhostPairCallback;
class btSequentialImpulseConstraintSolver;

class LocalServer;

struct PhysicsObject;
struct CharacterObject;

class BtPhysics : public PhysicsIface {
public:
  BtPhysics(LocalServer*);
  virtual ~BtPhysics();

  virtual void pushMessage(ServerMsg*);
  virtual void finish();

  virtual void addPhysical(uint32_t, const std::string&, float*, bool = false);
  virtual void delPhysical(uint32_t, bool = false);
  virtual void setInputState(uint32_t, uint16_t);

  virtual void setInputStateCallback(uint32_t, uint16_t);

  void run();
private:
  void parseMessage(ServerMsg*);
  void updateCharacterObjects();

  std::map<uint32_t, PhysicsObject*> m_objects;
  std::map<uint32_t, CharacterObject*> m_characters;
  queue<ServerMsg*> m_msg_queue;
  LocalServer* m_server;

  // thread management variables
  bool m_running, m_finish;
  boost::mutex m_status_mutex;
  boost::condition_variable m_status_cond;
  
  // bullet world classes
  btBroadphaseInterface* m_broadphase;
  btGhostPairCallback* m_gpcallback;
  btCollisionDispatcher* m_dispatcher;
  btDefaultCollisionConfiguration* m_collision_config;
  btDiscreteDynamicsWorld* m_world;
  btSequentialImpulseConstraintSolver* m_solver;
};

#endif // QNT_SERVER_BULLET_BYPHYSICS_HPP
