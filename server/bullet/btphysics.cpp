#include "btphysics.hpp"

#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "core/messages/server/inputmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"
#include "server/localserver.hpp"

#include <iostream>

namespace {
  class BulletInternalMsg : public ServerMsg {
  public:
    enum {
      AddPhysMessage = ServerMsg::SubsystemInternalStart,
      AddCharMessage,
      DelPhysMessage,
      DelCharMessage,
      ForceStateMessage,
    };
    BulletInternalMsg(uint32_t id) : ServerMsg(0), m_objid(id) {}
    virtual ~BulletInternalMsg() {}
    virtual void read(std::iostream&) {};
    virtual void write(std::iostream&) const {};

    uint32_t m_objid;
  };

  class AddPhysObjMsg : public BulletInternalMsg {
  public:
    AddPhysObjMsg(uint32_t id, const std::string& tmp, float *mat) : BulletInternalMsg(id), m_template(tmp) { memcpy(m_matrix, mat, 64); }
    virtual ~AddPhysObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::AddPhysMessage; }

    std::string m_template;
    float m_matrix[16];
  };

  class AddCharObjMsg: public AddPhysObjMsg {
  public:
    AddCharObjMsg(uint32_t id, const std::string& tmp, float *mat) : AddPhysObjMsg(id, tmp, mat) {}
    virtual ~AddCharObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::AddCharMessage; }
  };

  class DelPhysObjMsg : public BulletInternalMsg {
  public:
    DelPhysObjMsg(uint32_t id) : BulletInternalMsg(id) {}
    virtual ~DelPhysObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::DelPhysMessage; }
  };

  class DelCharObjMsg : public DelPhysObjMsg {
  public:
    DelCharObjMsg(uint32_t id) : DelPhysObjMsg(id) {}
    virtual ~DelCharObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::DelCharMessage; }
  };

  class ForceStateMsg : public BulletInternalMsg {
  public:
    ForceStateMsg(uint32_t id, uint16_t state) : BulletInternalMsg(id), m_state(state) {}
    virtual ~ForceStateMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::ForceStateMessage; }

    uint16_t m_state;
  };

  struct BulletLauncher {
    void operator()(BtPhysics* p) {
      p->run();
    }
  };

  struct planeParams {
    std::string name;
    btVector3 normal;
  };

  planeParams planes[] = {
    {"Floor", btVector3(0, 0, 1) },
    {"LWall", btVector3(1, 0, 0) },
    {"RWall", btVector3(-1, 0, 0) }
  };
}

struct PhysicsObject {
  btCollisionShape *m_collision;
  btRigidBody *m_body;
};

struct CharacterObject {
  btConvexShape *m_collision;
  btPairCachingGhostObject *m_object;
  // TODO: replace this with a dynamic controller
  btKinematicCharacterController *m_controller;
  uint16_t m_state;

  //TODO: store other information about this character: speed, turn rate, &c
};

BtPhysics::BtPhysics(LocalServer *srv) : m_server(srv) {
  m_running = false;
  m_finish = false;

  BulletLauncher launcher;
  boost::thread t(launcher, this);

  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
}

BtPhysics::~BtPhysics() {}

void BtPhysics::run() {
  btVector3 worldMin(-1000,-1000,-1000);
  btVector3 worldMax(1000,1000,1000);
  m_broadphase = new btAxisSweep3(worldMin,worldMax); //TODO: get bounds from world information
  m_collision_config = new btDefaultCollisionConfiguration;
  m_dispatcher = new btCollisionDispatcher(m_collision_config);
  m_solver = new btSequentialImpulseConstraintSolver;
  m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_config);
  m_world->setGravity(btVector3(0, 0, -9.8));
  m_gpcallback = new btGhostPairCallback();
  m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(m_gpcallback);

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  while(!m_finish) {
    boost::system_time t = boost::get_system_time();
    t += boost::posix_time::millisec(1000/30);

    ServerMsg* msg;
    while(m_msg_queue.try_pop(msg))
      parseMessage(msg);

    updateCharacterObjects();
    m_world->stepSimulation(1.f/30.f, 1, 1.f/30.f);

    LocalServer::ClientTransList ct;
    ct.reserve(m_characters.size());
    for(auto i = m_characters.begin(); i != m_characters.end(); ++i) {
      float *mat = new float[16];
      i->second->m_object->getWorldTransform().getOpenGLMatrix(mat);
      ct.push_back(std::make_pair(i->first, mat));
    }
    m_server->runTickCallbacks();
    m_server->pushClientTransforms(ct);

    boost::this_thread::sleep(t);
  }

  for(auto i = m_characters.begin(); i != m_characters.end(); ++i) {
    CharacterObject *character = i->second;
    m_world->removeAction(character->m_controller);
    m_world->removeCollisionObject(character->m_object);
    delete character->m_controller;
    delete character->m_object;
    delete character->m_collision;
    delete character;
  }
  for(auto i = m_objects.begin(); i != m_objects.end(); ++i) {
    PhysicsObject *object = i->second;
    m_world->removeRigidBody(object->m_body);
    delete object->m_body->getMotionState();
    delete object->m_body;
    delete object->m_collision;
    delete object;
  }

  delete m_world;
  delete m_solver;
  delete m_dispatcher;
  delete m_collision_config;
  delete m_broadphase;
  delete m_gpcallback;

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void BtPhysics::pushMessage(ServerMsg* msg) {
  m_msg_queue.push(msg);
}

void BtPhysics::finish() {
  m_finish = true;
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
}

void BtPhysics::addPhysical(uint32_t id, const std::string& tmp, float *mat, bool isCharacter) {
  if(isCharacter) {
    pushMessage(new AddCharObjMsg(id, tmp, mat));
  } else {
    pushMessage(new AddPhysObjMsg(id, tmp, mat));
  }
}

void BtPhysics::delPhysical(uint32_t id, bool isCharacter) {
  if(isCharacter) {
    pushMessage(new DelCharObjMsg(id));
  } else {
    pushMessage(new DelPhysObjMsg(id));
  }
}

void BtPhysics::setInputState(uint32_t id, uint16_t state) {
  pushMessage(new ForceStateMsg(id, state));
}

void BtPhysics::setInputStateCallback(uint32_t id, uint16_t state) {
  auto i = m_characters.find(id);
  if(i != m_characters.end())
    i->second->m_state = state;
}

void BtPhysics::parseMessage(ServerMsg *msg) {
  switch(msg->type()) {
    case BulletInternalMsg::ForceStateMessage: {
      ForceStateMsg *smsg = static_cast<ForceStateMsg*>(msg);
      setInputStateCallback(smsg->m_objid, smsg->m_state);
    } break;
    case ServerMsg::InputMessage: {
      ServerInputMsg *imsg = static_cast<ServerInputMsg*>(msg);
      uint32_t obj_id = m_server->getClientId(msg->m_sender);
      setInputStateCallback(obj_id, imsg->m_state);
    } break;
    case BulletInternalMsg::AddPhysMessage: {
      btVector3 normal;
      AddPhysObjMsg *amsg = static_cast<AddPhysObjMsg*>(msg);
      size_t i;
      for(i = 0; i < 3; ++i) {
        if(planes[i].name == amsg->m_template) {
          normal = planes[i].normal;
          break;
        }
      }
      if(i == 3) {
        std::cerr << "Unknown object type: " << amsg->m_template << std::endl;
        break;
      }
      PhysicsObject *object = new PhysicsObject;
      object->m_collision = new btStaticPlaneShape(normal, 1);
      btTransform trans;
      trans.setFromOpenGLMatrix(amsg->m_matrix);
      btDefaultMotionState *state = new btDefaultMotionState(trans);
      btRigidBody::btRigidBodyConstructionInfo ci(0.f, state, object->m_collision);
      object->m_body = new btRigidBody(ci);
      object->m_body->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
      object->m_body->setFriction(0.f);
      m_world->addRigidBody(object->m_body);
      m_objects[amsg->m_objid] = object;
    } break;
    case BulletInternalMsg::AddCharMessage: {
      AddCharObjMsg *cmsg = static_cast<AddCharObjMsg*>(msg);
      CharacterObject *character = new CharacterObject;
      character->m_state = 0;
      btTransform start;
      start.setIdentity();
      character->m_object = new btPairCachingGhostObject();
      character->m_object->setWorldTransform(start);
      character->m_collision = new btCapsuleShape(1.f, 0.f);
      character->m_object->setCollisionShape(character->m_collision);
      character->m_object->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
      btTransform trans;
      trans.setFromOpenGLMatrix(cmsg->m_matrix);
      character->m_object->setWorldTransform(trans);
      character->m_object->setFriction(0.f);
      character->m_controller = new btKinematicCharacterController(character->m_object, character->m_collision, 0.35f);
      character->m_controller->setUpAxis(2);
      character->m_controller->setGravity(9.8f);
      m_world->addCollisionObject(character->m_object,btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);
      m_world->addAction(character->m_controller);
      m_characters[cmsg->m_objid] = character;
    } break;
    case BulletInternalMsg::DelPhysMessage: {
      DelPhysObjMsg *dmsg = static_cast<DelPhysObjMsg*>(msg);
      PhysicsObject *object = m_objects[dmsg->m_objid];
      m_objects.erase(dmsg->m_objid);
      m_world->removeRigidBody(object->m_body);
      delete object->m_body->getMotionState();
      delete object->m_body;
      delete object->m_collision;
      delete object;
    } break;
    case BulletInternalMsg::DelCharMessage: {
      DelCharObjMsg *cmsg = static_cast<DelCharObjMsg*>(msg);
      CharacterObject *character = m_characters[cmsg->m_objid];
      m_characters.erase(cmsg->m_objid);
      m_world->removeAction(character->m_controller);
      m_world->removeCollisionObject(character->m_object);
      delete character->m_controller;
      delete character->m_object;
      delete character->m_collision;
      delete character;
    } break;
    default:
      break;
  }
  delete msg;
}

void BtPhysics::updateCharacterObjects() {
  for(auto i = m_characters.begin(); i != m_characters.end(); ++i) {
    CharacterObject *character = i->second;
    if(!character->m_controller->canJump())
      continue;
    btTransform trans = character->m_object->getWorldTransform();
    btVector3 forward = trans.getBasis()[2];
    btVector3 up = trans.getBasis()[1];
    btVector3 side = trans.getBasis()[0];
    forward.normalize();
    up.normalize();
    side.normalize();

    btVector3 direction(0.f, 0.f, 0.f);
    btScalar speed = 7.f / 30.f; // 7 units/second for fixed 30 ticks / second physics rate
    if(character->m_state & ServerInputMsg::StrafeLeft)
      direction -= side;
    if(character->m_state & ServerInputMsg::StrafeRight)
      direction += side;
    if(character->m_state & ServerInputMsg::WalkForward)
      direction += forward;
    if(character->m_state & ServerInputMsg::WalkReverse)
      direction -= forward;
    if(character->m_state & ServerInputMsg::TurnLeft) {
      btMatrix3x3 basis = character->m_object->getWorldTransform().getBasis();
      basis *= btMatrix3x3(btQuaternion(btVector3(0,1,0),0.01));
      character->m_object->getWorldTransform ().setBasis(basis);
    }
    if(character->m_state & ServerInputMsg::TurnRight) {
      btMatrix3x3 basis = character->m_object->getWorldTransform().getBasis();
      basis *= btMatrix3x3(btQuaternion(btVector3(0,1,0),-0.01));
      character->m_object->getWorldTransform ().setBasis(basis);
    }
    if(character->m_state & ServerInputMsg::Jump && character->m_controller->canJump()) {
      character->m_controller->jump();
    }
    if(character->m_state & ServerInputMsg::Shoot) {
      //TODO: launch projectile physical
    }

    character->m_controller->setWalkDirection(direction * speed);
  }
}
