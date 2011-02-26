#ifndef QNT_CORE_CLIENTIFACE_HPP
#define QNT_CORE_CLIENTIFACE_HPP

class ClientMsg;
class ServerIface;

class ClientIface {
public:
  virtual ~ClientIface();
  virtual ServerIface* server() const =0;
  virtual void pushMessage(ClientMsg*) =0;
  virtual void serverClosed() =0;
};

#endif // QNT_CORE_CLIENTIFACE_HPP
