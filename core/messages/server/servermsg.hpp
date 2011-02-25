#ifndef QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP
#define QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP

#include <iosfwd>
#include <boost/cstdint.hpp>

class ClientIface;

class ServerMsg {
public:
  enum MessageType {
    ShutdownMessage,
    InputMessage,
    SubsystemInternalStart,
    SubsystemInternalEnd = SubsystemInternalStart + 0x200, // gives us 512 possible internal messages for each subsystem.
  };

  ServerMsg(ClientIface*);
  virtual ~ServerMsg();
  virtual MessageType type() const =0;
  virtual void read(std::iostream&) =0;
  virtual void write(std::iostream&) const =0;

  ClientIface* m_sender;
  
  static ServerMsg *create(uint32_t, ClientIface*);
};

#endif // QNT_CORE_MESSAGES_SERVER_SERVERMSG_HPP
