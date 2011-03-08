#ifndef QNT_CORE_CLIENTIFACE_HPP
#define QNT_CORE_CLIENTIFACE_HPP

class ClientMsg;
class ServerIface;

/// \brief Client thread interface
/// \defgroup Client Client Classes
/// Clients handle audio and rendering, as well as user input processing.
/// They can be in-process or external. Subclasses determine all implementation details.
class ClientIface {
public:
  virtual ~ClientIface();

  /// \brief The server tied to this client
  virtual ServerIface* server() const =0;

  /// \brief Add a message to the client queue
  /// Every frame this queue is emptied, and the messages are dispatched
  /// to the correct client subsystem. There may be one (or more) network
  /// or IPC layers between this call and the actual queue.
  /// \param msg The message to dispatch
  virtual void pushMessage(ClientMsg*) =0;

  /// \brief Called on server shutdown
  /// This function is called by the server (local or remote) on shutdown.
  /// It signals that the main loop should be terminated. There may be one
  /// (or more) network or IPC layers between this call and the actual mainloop.
  virtual void serverClosed() =0;
};

#endif // QNT_CORE_CLIENTIFACE_HPP
