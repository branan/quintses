#ifndef QNT_CLIENT_PLATFORMDATA_X11_H
#define QNT_CLIENT_PLATFORMDATA_X11_H

#include <X11/Xlib.h>
#include <GL/glx.h>

struct PlatformData {
  Display *dpy;
  Window win;
  GLXContext ctx;
  GLXFBConfig fbc;

  // the use of separate anything assumes multi-threading
  // alias the context if you need to
  GLXContext ldr_ctx;
  GLXPbuffer ldr_draw;
};

#endif // QNT_CLIENT_PLATFORMDATA_X11_H
