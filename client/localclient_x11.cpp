#include "platformdata_x11.hpp" // included first to keep X11 headers in order
#include <GL/glxext.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdexcept>

#include "core/serveriface.hpp"
#include "core/messages/server/inputmsg.hpp"
#include "localclient.hpp"
#include "opengl/glloader.hpp"
#include "opengl/glrender.hpp"

namespace {
  Atom WM_PROTOCOLS;
  Atom WM_DELETE_WINDOW;

  Bool predicate(Display*, XEvent*, XPointer) {
    return 1;
  }
} // namespace

void LocalClient::initializePlatform() {

  // TODO: get information about the window layout from a config system of some sort
  const int width = 800;
  const int height = 600;
  const char* name = "Quintses";

  m_platform = new PlatformData;
  m_platform->dpy = XOpenDisplay(0);
  if(!m_platform->dpy) {
    delete m_platform;
    throw std::runtime_error("Could not open X11 Display");
  }
  WM_PROTOCOLS = XInternAtom(m_platform->dpy, "WM_PROTOCOLS", False);
  WM_DELETE_WINDOW = XInternAtom(m_platform->dpy, "WM_DELETE_WINDOW", False);
  int nelements;
  GLXFBConfig* configs = glXChooseFBConfig(m_platform->dpy, DefaultScreen(m_platform->dpy),
                                           glx_attributes, &nelements);
  if(0 == nelements) {
    XCloseDisplay(m_platform->dpy);
    delete m_platform;
    throw std::runtime_error("No OpenGL framebuffer configuration found");
  }
  GLXFBConfig fbc = configs[0];
  XFree(configs);
  XVisualInfo* vi = glXGetVisualFromFBConfig(m_platform->dpy, fbc);
  if(!vi) {
    XCloseDisplay(m_platform->dpy);
    delete m_platform;
    throw std::runtime_error("Could not get X11 visual from FB config");
  }
  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap(m_platform->dpy, RootWindow(m_platform->dpy, vi->screen),
                                 vi->visual, AllocNone);
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask;
  m_platform->win = XCreateWindow(m_platform->dpy, RootWindow(m_platform->dpy, vi->screen), 0, 0,
                                  width, height, 0, vi->depth, InputOutput, vi->visual,
                                  CWBorderPixel|CWColormap|CWEventMask, &swa);
  XFree(vi);
  if(!m_platform->win) {
    XCloseDisplay(m_platform->dpy);
    delete m_platform;
    throw std::runtime_error("Could not open the X11 window");
  }
  XSizeHints sizehint;
  sizehint.min_width = sizehint.max_width = sizehint.base_width = sizehint.width = width;
  sizehint.min_height = sizehint.max_height = sizehint.base_height = sizehint.height = height;
  sizehint.flags = PSize | PMinSize | PMaxSize | PBaseSize;
  XSetWMNormalHints(m_platform->dpy, m_platform->win, &sizehint);
  XTextProperty namehint;
  XStringListToTextProperty(const_cast<char**>(&name), 1, &namehint);
  XSetWMName(m_platform->dpy, m_platform->win, &namehint);
  XFree(namehint.value);
  Atom protocols[] = { WM_DELETE_WINDOW };
  XSetWMProtocols(m_platform->dpy, m_platform->win, protocols, 1);
  m_platform->ctx = glXCreateNewContext(m_platform->dpy, fbc, GLX_RGBA_TYPE, 0, True);
  if(!m_platform->ctx) {
    XDestroyWindow(m_platform->dpy, m_platform->win);
    XCloseDisplay(m_platform->dpy);
    delete m_platform;
    throw std::runtime_error("Could not create an OpenGL context");
  }
  XMapWindow(m_platform->dpy, m_platform->win);
  glXMakeContextCurrent(m_platform->dpy, m_platform->win, m_platform->win, m_platform->ctx);

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(60.0, 0.75, 0.1, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glClear(GL_COLOR_BUFFER_BIT);
  glXSwapBuffers(m_platform->dpy, m_platform->win);

  m_loader = new GlLoader(m_platform);
  m_renderer = new GlRender(m_loader);
}

void LocalClient::finalizePlatform() {
  m_loader->finish();
  delete m_loader;
  delete m_renderer;
  glXDestroyContext(m_platform->dpy, m_platform->ctx);
  XDestroyWindow(m_platform->dpy, m_platform->win);
  XCloseDisplay(m_platform->dpy);
  delete m_platform;
}

void LocalClient::platformSwapBuffer() {
  glXSwapBuffers(m_platform->dpy, m_platform->win);
}

namespace {
  uint16_t keymap = 0;
  bool key_press;
  void adjustKeysym(uint16_t key) {
    key_press ? keymap |= key
          : keymap &= ~key;
  }
}
void LocalClient::platformEvents() {
  XEvent xevt;
  bool keys_changed = false;
  while(XCheckIfEvent(m_platform->dpy, &xevt, predicate, 0)) {
    switch(xevt.type) {
      case ClientMessage:
        if(xevt.xclient.message_type == WM_PROTOCOLS &&
          (unsigned long)xevt.xclient.data.l[0] == WM_DELETE_WINDOW) {
          m_finished = true;
        }
        break;
      case KeyRelease:
        key_press = false;
        goto ProcessKey;
      case KeyPress:
        key_press = true;
      ProcessKey:        
        switch(XKeycodeToKeysym(m_platform->dpy, xevt.xkey.keycode, 0)) {
          case XK_Left:
            adjustKeysym(ServerInputMsg::StrafeLeft);
            keys_changed = true;
            break;
          case XK_Right:
            adjustKeysym(ServerInputMsg::StrafeRight);
            keys_changed = true;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
  if(keys_changed) {
    ServerInputMsg *imsg = new ServerInputMsg(this);
    imsg->m_state = keymap;
    m_server->pushMessage(imsg);
  }
}
