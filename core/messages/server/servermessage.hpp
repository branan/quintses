#ifndef QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP
#define QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP

class ServerMessage {
public:
  virtual ~ServerMessage();

  enum MessageType {
    ShutdownMessage
  };
  virtual MessageType type() const =0;
};

#endif // QNT_CORE_SERVER_MESSAGES_SERVERMESSAGE_HPP
