#ifndef QNT_CLIENT_PLATFORMDATA_X11_H
#define QNT_CLIENT_PLATFORMDATA_X11_H

#include <X11/Xlib.h>
#include <GL/glx.h>

struct PlatformData {
  Display *dpy;
  Window win;
  GLXContext ctx;

  // the use of separate anything assumes multi-threading
  // alias the context if you need to
  Display *ldr_dpy;
  GLXContext ldr_ctx;
  GLXPbuffer ldr_draw;
};

const int glx_attributes[] = {
  GLX_DOUBLEBUFFER, 1,
  GLX_STENCIL_SIZE, 8,
  GLX_RENDER_TYPE, GLX_RGBA_BIT,
  GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT | GLX_PBUFFER_BIT,
  0, 0
};

#endif // QNT_CLIENT_PLATFORMDATA_X11_H
