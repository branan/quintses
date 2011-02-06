#ifndef QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP
#define QNT_CORE_MESSAGES_SERVER_SERVERMESSAGE_HPP

#include <iosfwd>

class ClientIface;

class ServerMessage {
public:
  ServerMessage(ClientIface*);
  virtual ~ServerMessage();

  enum MessageType {
    ShutdownMessage,
  };
  virtual MessageType type() const =0;
  virtual size_t size() const =0;

  ClientIface* sender() const { return m_sender; }

private:
  ClientIface* m_sender;
};

#endif // QNT_CORE_SERVER_MESSAGES_SERVERMESSAGE_HPP
