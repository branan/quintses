#ifndef QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP
#define QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP

#include <boost/cstdint.hpp>

class SocketWrapper;
class ClientIface;

class ServerMsg {
public:
  enum MessageType {
    ShutdownMessage,
    InputMessage,
    LoadPlayer,
    SubsystemInternalStart,
    SubsystemInternalEnd = SubsystemInternalStart + 0x200, // gives us 512 possible internal messages for each subsystem.
  };

  ServerMsg(ClientIface*);
  virtual ~ServerMsg();
  virtual MessageType type() const =0;
  virtual void read(SocketWrapper&) =0;
  virtual void write(SocketWrapper&) const =0;

  ClientIface* m_sender;
  
  static ServerMsg *create(uint32_t, ClientIface*);
};

#endif // QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP
