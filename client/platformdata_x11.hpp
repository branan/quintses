#ifndef QNT_CLIENT_PLATFORMDATA_X11_H
#define QNT_CLIENT_PLATFORMDATA_X11_H

#include <X11/Xlib.h>

struct PlatformData {
  Display *dpy;
  Window w;
};

#endif // QNT_CLIENT_PLATFORMDATA_X11_H
