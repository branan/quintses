#ifndef QNT_CORE_RENDERIFACE_HPP
#define QNT_CORE_RENDERIFACE_HPP

#include <string>

class ClientMsg;

class RenderIface {
public:
  virtual ~RenderIface();

  virtual void pushMessage(ClientMsg*) =0;
  virtual void draw() const =0;
};

#endif // QNT_CORE_RENDERIFACE_HPP
