#ifndef QNT_CLIENT_LOCALCLIENT_H
#define QNT_CLIENT_LOCALCLIENT_H

#include "core/clientiface.hpp"
#include "core/util/queue.hpp"

// structure to hold platform-specific fields
struct PlatformData;

class AudioIface;
class LoaderIface;
class RenderIface;

class LocalClient : public ClientIface {
public:
  LocalClient(ServerIface *);
  ~LocalClient();

  virtual ServerIface* server() const;
  virtual void pushMessage(ClientMsg*);
  virtual void serverClosed() { m_finished = true; }

  int mainloop();

private:
  void initializePlatform();
  void finalizePlatform();
  void platformEvents();
  void platformSwapBuffer();

  ServerIface* m_server;
  LoaderIface* m_loader;
  AudioIface* m_audio;
  RenderIface* m_renderer;
  PlatformData* m_platform;
  bool m_finished;

  queue<ClientMsg*> m_msg_queue;
};

#endif // QNT_CLIENT_LOCALCLIENT_H
