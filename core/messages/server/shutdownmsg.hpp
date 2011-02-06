#ifndef QNT_CORE_MESSAGES_SERVER_SHUTDOWNMESSAGE_HPP
#define QNT_CORE_MESSAGES_SERVER_SHUTDOWNMESSAGE_HPP

#include "servermessage.hpp"

class ServerShutdownMsg : public ServerMessage {
public:
  ServerShutdownMsg(ClientIface*);
  virtual ~ServerShutdownMsg();
  virtual MessageType type() const;
  virtual size_t size() const;
};

#endif // QNT_CORE_SERVER_MESSAGES_SHUTDOWNMESSAGE_HPP
