#ifndef QNT_CLIENT_LOCALCLIENT_H
#define QNT_CLIENT_LOCALCLIENT_H

#include "core/clientiface.hpp"

// structure to hold platform-specific fields
struct PlatformData;

class LocalClient : public ClientIface {
public:
  LocalClient();
  ~LocalClient();

  virtual ServerIface* server() const;
  int mainloop();

private:
  void initializePlatform();
  void finalizePlatform();

  ServerIface* m_server;
  PlatformData* m_platform;
};

#endif // QNT_CLIENT_LOCALCLIENT_H
