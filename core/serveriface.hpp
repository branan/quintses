#ifndef QNT_CORE_SERVERIFACE_H
#define QNT_CORE_SERVERIFACE_H

class ClientIface;
class ServerMsg;

/// \brief Interface to game servers
/// \defgroup Server Server Classes
/// The server runs physics, AI, and general gameplay scripting.
/// Servers may be local or remote. Their interface is always asynchronous.
class ServerIface {
public:
  virtual ~ServerIface();

  // client management
  /// \brief Add a new client
  /// This adds the client to the list of lobby clients. Lobby clients receive game updates but
  /// do not interact as player characters. To add a character for a client, send the server
  /// a ServerLoadPlayerMsg.
  virtual void addClient(ClientIface*) =0;

  /// \brief Remove a client
  /// If there is a player object for the client it is deleted
  virtual void removeClient(ClientIface*) =0;

  /// Privileged clients will be able to shut down the server and do other such magic.
  /// Currently, there are no privilege checks. All clients have full power.
  virtual void makeClientPrivileged(ClientIface*) =0;

  /// \return True if this is a server running in the same process as the caller
  virtual bool isLocal() const =0;

  // Server message interface
  /// \brief add a message to the server queue
  virtual void pushMessage(ServerMsg*) =0;

  // General management
  /// Wait for the server thread to finish. For non-local servers, this function
  /// may signal the socket or IPC system to close. For local servers, this
  /// function must block
  virtual int waitForTermination() const =0;
};

#endif // QNT_CORE_SERVERIFACE_H
