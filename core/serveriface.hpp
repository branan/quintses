#ifndef QNT_CORE_SERVERIFACE_H
#define QNT_CORE_SERVERIFACE_H

class ClientIface;

class ServerIface {
public:
  virtual ~ServerIface();

  virtual void addClient(ClientIface*) =0;

  virtual int waitForTermination() =0;
};

#endif // QNT_SERVER_SERVERIFACE_H
