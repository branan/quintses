#ifndef QNT_CLIENT_LOCALCLIENT_H
#define QNT_CLIENT_LOCALCLIENT_H

#include "core/clientiface.hpp"
#include "core/util/queue.hpp"

// structure to hold platform-specific fields
struct PlatformData;

class AudioIface;
class LoaderIface;
class RenderIface;

/// \brief A client running in the current process
/// \ingroup Client
/// This client handles passing messages from the server to the appropriate
/// client-side subsystems. Its thread also contains the input and rendering.
class LocalClient : public ClientIface {
public:
  /// \brief Construct a new LocalClient tied to the given ServerIface
  /// This creates the main window, initializes all of the client-side
  /// subsystems, and adds this client to the server.
  /// \param serv The server to tie this client to
  LocalClient(ServerIface *serv);

  /// \brief Cleanup
  /// 
  ~LocalClient();

  /// \brief Main game loop
  /// This function should be called from the top-level application function.
  /// It does not return until the server or user signals a shutdown event.
  /// \return A status value usable as an application exit code.
  int mainloop();

  // reimplemented functions
  virtual ServerIface* server() const;
  virtual void pushMessage(ClientMsg *msg);
  virtual void serverClosed() { m_finished = true; }
private:
  /// \brief Platform-specific initialization code
  /// This creates the main window, the renderer, and the graphics loader.
  void initializePlatform();

  /// \brief Platform-specific cleanup
  /// This closes the application window and cleans up any global graphics resources
  void finalizePlatform();

  /// \brief Run platform event loop
  /// This removes all pending events from the application event queue and processes them.
  /// It will send the appropriate messages to other subsystems or the server as needed.
  void platformEvents();

  /// \brief Swap buffers
  /// If the rendering system is double-buffered, this will invoke the platform-specific
  /// bufferswap function. It may be a noop if a given platform is single buffered.
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
