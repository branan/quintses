#ifndef QNT_CORE_PHYSICSIFACE_HPP
#define QNT_CORE_PHYSICSIFACE_HPP

class PhysicsIface {
public:
  virtual ~PhysicsIface();

  virtual void finish() =0;
};

#endif // QNT_CORE_PHYSICSIFACE_HPP
