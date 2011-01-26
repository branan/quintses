#ifndef QNT_CLIENT_LOCALCLIENT_H
#define QNT_CLIENT_LOCALCLIENT_H

#include "core/clientiface.hpp"

class LocalClient : public ClientIface {
public:
  LocalClient();
  virtual ServerIface* server() const;
  int mainloop();

private:
  ServerIface* m_server;
};

#endif // QNT_CLIENT_LOCALCLIENT_H