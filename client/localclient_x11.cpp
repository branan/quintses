#include "localclient.hpp"
#include "platformdata_x11.hpp"

void LocalClient::initializePlatform() {
  m_platform = new PlatformData;
  m_platform->dpy = XOpenDisplay(0);
}

void LocalClient::finalizePlatform() {
  XCloseDisplay(m_platform->dpy);
  delete m_platform;
}