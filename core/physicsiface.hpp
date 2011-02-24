#ifndef QNT_CORE_PHYSICSIFACE_HPP
#define QNT_CORE_PHYSICSIFACE_HPP

#include <boost/cstdint.hpp>

class ServerMsg;

class PhysicsIface {
public:
  virtual ~PhysicsIface();

  virtual void pushMessage(ServerMsg*) =0;
  virtual void finish() =0;

  virtual void addPlayerPhysical(uint32_t) =0;
  virtual void delPlayerPhysical(uint32_t) =0;
};

#endif // QNT_CORE_PHYSICSIFACE_HPP
