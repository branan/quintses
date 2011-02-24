#include "btphysics.hpp"

#include "btBulletDynamicsCommon.h"

#include "core/messages/server/inputmsg.hpp"
#include "server/localserver.hpp"

namespace {
  class BulletInternalMsg : public ServerMsg {
  public:
    enum {
      AddPhysMessage = ServerMsg::SubsystemInternalStart,
      DelPhysMessage
    };
    BulletInternalMsg(uint32_t id) : ServerMsg(0), m_objid(id) {}
    virtual ~BulletInternalMsg() {}
    virtual void read(std::iostream&) {};
    virtual void write(std::iostream&) const {};

    uint32_t m_objid;
  };

  class AddPhysObjMsg : public BulletInternalMsg {
  public:
    AddPhysObjMsg(uint32_t id) : BulletInternalMsg(id) {}
    virtual ~AddPhysObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::AddPhysMessage; }
  };

  class DelPhysObjMsg : public BulletInternalMsg {
  public:
    DelPhysObjMsg(uint32_t id) : BulletInternalMsg(id) {}
    virtual ~DelPhysObjMsg() {}
    virtual MessageType type() const { return (MessageType)BulletInternalMsg::DelPhysMessage; }
  };

  struct BulletLauncher {
    void operator()(BtPhysics* p) {
      p->run();
    }
  };
}

struct PhysicsObject {
  uint16_t m_state;
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
  m_broadphase = new btDbvtBroadphase;
  m_collision_config = new btDefaultCollisionConfiguration;
  m_dispatcher = new btCollisionDispatcher(m_collision_config);
  m_solver = new btSequentialImpulseConstraintSolver;
  m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collision_config);
  m_world->setGravity(btVector3(0, -9.8, 0));

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  while(!m_finish) {
    boost::system_time t = boost::get_system_time();
    t += boost::posix_time::millisec(1000/30);
    ServerMsg* msg;
    while(m_msg_queue.try_pop(msg)) {
      switch(msg->type()) {
        case ServerMsg::InputMessage: {
          ServerInputMsg *imsg = static_cast<ServerInputMsg*>(msg);
          uint32_t obj_id = m_server->getClientId(msg->m_sender);
          m_objects[obj_id]->m_state = imsg->m_state;
        }
          break;
        case BulletInternalMsg::AddPhysMessage: {
            AddPhysObjMsg *amsg = static_cast<AddPhysObjMsg*>(msg);
            m_objects[amsg->m_objid] = new PhysicsObject;
          }
          break;
        case BulletInternalMsg::DelPhysMessage: {
          DelPhysObjMsg *dmsg = static_cast<DelPhysObjMsg*>(msg);
          m_objects[dmsg->m_objid] = new PhysicsObject;
        }
        break;
        default:
          break;
      }
      delete msg;
    }
    m_world->stepSimulation(1.f/30.f, 1, 1.f/30.f);
    boost::this_thread::sleep(t);
  }

  delete m_world;
  delete m_solver;
  delete m_dispatcher;
  delete m_collision_config;
  delete m_broadphase;

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

void BtPhysics::addPlayerPhysical(uint32_t id) {
  pushMessage(new AddPhysObjMsg(id));
}

void BtPhysics::delPlayerPhysical(uint32_t id) {
  pushMessage(new DelPhysObjMsg(id));
}
