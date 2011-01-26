#ifndef QNT_CLIENT_PLATFORMDATA_X11_H
#define QNT_CLIENT_PLATFORMDATA_X11_H

#include <X11/Xlib.h>
#include <GL/glx.h>

struct PlatformData {
  Display *dpy;
  Window win;
  GLXContext ctx;
};

#endif // QNT_CLIENT_PLATFORMDATA_X11_H
