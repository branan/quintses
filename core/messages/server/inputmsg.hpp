#ifndef QNT_CORE_MESSAGES_SERVER_INPUTMSG_HPP
#define QNT_CORE_MESSAGES_SERVER_INPUTMSG_HPP

#include "servermsg.hpp"

class ServerInputMsg : public ServerMsg {
public:
  enum State {
    StrafeLeft  = 1 << 0,
    StrafeRight = 1 << 1,
    WalkForward = 1 << 2,
    WalkReverse = 1 << 3,
    TurnLeft = 1 << 4,
    TurnRight = 1 << 5,
    Jump = 1 << 6,
    Shoot = 1 << 7,
  };
  ServerInputMsg(ClientIface* cli) : ServerMsg(cli) {}
  virtual ~ServerInputMsg();
  virtual MessageType type() const;
  virtual void read(SocketWrapper&);
  virtual void write(SocketWrapper&) const;

  uint16_t m_state;
};

#endif // QNT_CORE_MESSAGES_SERVER_INPUTMSG_HPP
