#ifndef QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP
#define QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP

#include <iosfwd>

class ServerMessage {
public:
  virtual ~ServerMessage();

  enum MessageType {
    ShutdownMessage,
    ForceSize32 = 0xFFFFFFFF
  };
  virtual MessageType type() const =0;

  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;
};

#endif // QNT_CORE_SERVER_MESSAGES_SERVERMESSAGE_HPP
