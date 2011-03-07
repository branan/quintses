#ifndef QNT_CORE_PHYSICSIFACE_HPP
#define QNT_CORE_PHYSICSIFACE_HPP

#include <boost/cstdint.hpp>
#include <string>

class ServerMsg;

class PhysicsIface {
public:
  virtual ~PhysicsIface();

  virtual void pushMessage(ServerMsg*) =0;
  virtual void finish() =0;

  virtual void addPhysical(uint32_t, const std::string&, float*, bool = false) =0;
  virtual void delPhysical(uint32_t, bool = false) =0;
  virtual void setInputState(uint32_t, uint16_t) =0;

  // the Callback versions of functions run immediately, rather than being added to the queue. They're only safe during physics callbacks
  virtual void setInputStateCallback(uint32_t, uint16_t) =0;
};

#endif // QNT_CORE_PHYSICSIFACE_HPP
