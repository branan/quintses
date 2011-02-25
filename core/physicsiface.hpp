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

  virtual void addCharacter(uint32_t, const std::string&) =0;
  virtual void addPhysical(uint32_t, const std::string&) =0;
  virtual void delCharacter(uint32_t) =0;
  virtual void delPhysical(uint32_t) =0;
};

#endif // QNT_CORE_PHYSICSIFACE_HPP
