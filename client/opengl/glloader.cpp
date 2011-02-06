#include "glloader.hpp"

namespace {
  struct LoaderLauncher {
    void operator()(GlLoader* l) {
      l->run();
    }
  };
}

GlLoader::GlLoader(PlatformData* pd) {
  m_running = false;
  m_finish = false;
  m_platform = pd;
  LoaderLauncher launcher;
  boost::thread t(launcher, this);

  // wait for the thread to finish initializing
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
}

GlLoader::~GlLoader() {}

void GlLoader::run() {
  initializePlatform();
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  while(!m_finish) {
    //TODO: check queue for loading jobs
    boost::this_thread::yield();
  }

  finalizePlatform();
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void GlLoader::finish() {
  m_finish = true;
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
}

