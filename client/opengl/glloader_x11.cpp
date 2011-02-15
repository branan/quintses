#include "client/platformdata_x11.hpp" // included first to keep X11 headers in order
#include <GL/glxext.h>
#include <GL/gl.h>

#include <stdexcept>

#include "glloader.hpp"

namespace {
  const int pbuffer_attributes[] = {
    GLX_PBUFFER_WIDTH, 1,
    GLX_PBUFFER_HEIGHT, 1,
    GLX_PRESERVED_CONTENTS, False,
    0, 0
  };
}

void GlLoader::initializePlatform() {
  m_platform->ldr_dpy = XOpenDisplay(0);
  int nelements;
  GLXFBConfig* configs = glXChooseFBConfig(m_platform->ldr_dpy, DefaultScreen(m_platform->dpy),
                                           glx_attributes, &nelements);
  if(0 == nelements) {
    XCloseDisplay(m_platform->dpy);
    delete m_platform;
    throw std::runtime_error("No OpenGL framebuffer configuration found");
  }
  GLXFBConfig fbc = configs[0];
  XFree(configs);
  m_platform->ldr_draw = glXCreatePbuffer(m_platform->ldr_dpy, fbc, pbuffer_attributes);
  if(!m_platform->ldr_draw) {
    throw std::runtime_error("Could not create pbuffer for loader thread");
  }
  m_platform->ldr_ctx = glXCreateNewContext(m_platform->ldr_dpy, fbc, GLX_RGBA_TYPE, m_platform->ctx, True);
  if(!m_platform->ldr_ctx) {
    glXDestroyPbuffer(m_platform->ldr_dpy, m_platform->ldr_draw);
    throw std::runtime_error("Could not create context for loader thread");
  }
  glXMakeContextCurrent(m_platform->ldr_dpy, m_platform->ldr_draw, m_platform->ldr_draw, m_platform->ldr_ctx);
}

void GlLoader::finalizePlatform() {
  glXDestroyContext(m_platform->ldr_dpy, m_platform->ldr_ctx);
  glXDestroyPbuffer(m_platform->ldr_dpy, m_platform->ldr_draw);
  XCloseDisplay(m_platform->ldr_dpy);
}
