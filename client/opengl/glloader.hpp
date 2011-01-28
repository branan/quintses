#ifndef QNT_CLIENT_OPENGL_GLLOADER_H
#define QNT_CLIENT_OPENGL_GLLOADER_H

#include <boost/thread.hpp>

#include "core/loaderiface.hpp"

struct PlatformData;

class GlLoader : public LoaderIface {
public:
  GlLoader(PlatformData*);
  virtual ~GlLoader();

  virtual void finish();

  void run();

private:
  void initializePlatform();
  void finalizePlatform();

  PlatformData* m_platform;

  // threading management
  bool m_running;
  bool m_finish;
  boost::condition_variable m_status_cond;
  boost::mutex m_status_mutex;
};

#endif // QNT_CLIENT_OPENGL_GLLOADER_H