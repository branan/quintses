#ifndef QNT_CORE_SERVERIFACE_H
#define QNT_CORE_SERVERIFACE_H

class ClientIface;
class ServerMessage;

class ServerIface {
public:
  virtual ~ServerIface();

  // client management
  virtual void addClient(ClientIface*) =0;
  virtual void makeClientPrivileged(ClientIface*) =0;
  virtual bool isLocal() const =0;

  // Server message interface
  virtual void pushMessage(ServerMessage*) =0;

  // General management
  virtual int waitForTermination() const =0;
};

#endif // QNT_CORE_SERVERIFACE_H
