#ifndef QNT_CORE_CLIENTIFACE_HPP
#define QNT_CORE_CLIENTIFACE_HPP

class ServerIface;

class ClientIface {
public:
  virtual ~ClientIface();
  virtual ServerIface* server() const =0;
};

#endif // QNT_CORE_CLIENTIFACE_HPP
