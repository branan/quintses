#include "btphysics.hpp"

#include "btBulletDynamicsCommon.h"

namespace {
  struct BulletLauncher {
    void operator()(BtPhysics* p) {
      p->run();
    }
  };
}

BtPhysics::BtPhysics() {
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
    //TODO: process any physics-thread messages from the main server
    t += boost::posix_time::millisec(1000/30);
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

void BtPhysics::finish() {
  m_finish = true;
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
}
