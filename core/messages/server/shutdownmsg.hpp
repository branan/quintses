#ifndef QNT_CORE_MESSAGES_SERVER_SHUTDOWNMESSAGE_HPP
#define QNT_CORE_MESSAGES_SERVER_SHUTDOWNMESSAGE_HPP

#include "servermsg.hpp"

class ServerShutdownMsg : public ServerMsg {
public:
  ServerShutdownMsg(ClientIface*);
  virtual ~ServerShutdownMsg();
  virtual MessageType type() const;
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;
};

#endif // QNT_CORE_MESSAGES_SERVER_SHUTDOWNMESSAGE_HPP
