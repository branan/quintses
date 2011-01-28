#ifndef QNT_CLIENT_LOCALCLIENT_H
#define QNT_CLIENT_LOCALCLIENT_H

#include "core/clientiface.hpp"

// structure to hold platform-specific fields
struct PlatformData;

class LoaderIface;

class LocalClient : public ClientIface {
public:
  LocalClient();
  ~LocalClient();

  virtual ServerIface* server() const;
  int mainloop();

private:
  void initializePlatform();
  void finalizePlatform();
  void platformEvents();
  void platformSwapBuffer();

  ServerIface* m_server;
  LoaderIface* m_loader;
  PlatformData* m_platform;
  bool m_finished;
};

#endif // QNT_CLIENT_LOCALCLIENT_H
