#include "alaudio.hpp"
#include "streamingwav.hpp"

#include <al.h>
#include <alc.h>

namespace {
  struct OpenalLauncher {
    void operator()(AlAudio* a) {
      a->run();
    }
  };
}

AlAudio::AlAudio(){
  m_running = false;
  m_finish = false;
  
  OpenalLauncher launcher;
  boost::thread t(launcher, this);
  
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
}

AlAudio::~AlAudio() {}

void AlAudio::run() {
//   m_device = alcOpenDevice(0);
//   m_context = alcCreateContext(m_device, 0);
//   alcMakeContextCurrent(m_context);

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  while(!m_finish) {
    for(auto i = m_streams.begin(); i != m_streams.end();) {
      auto current = i++;
      if(!(*current)->update()) {
        delete (*current);
        m_streams.erase(current);
      }
    }
  }
  for(auto i = m_streams.begin(); i != m_streams.end(); ++i)
    delete (*i);

//   alcMakeContextCurrent(0);
//   alcDestroyContext(m_context);
//   alcCloseDevice(m_device);

  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void AlAudio::finish() {
  m_finish = true;
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
}
