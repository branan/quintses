#include "shutdownmsg.hpp"

ServerShutdownMsg::ServerShutdownMsg(ClientIface* sender): ServerMsg(sender) {}
ServerShutdownMsg::~ServerShutdownMsg() {}

ServerMsg::MessageType ServerShutdownMsg::type() const {
  return ServerMsg::ShutdownMessage;
}

void ServerShutdownMsg::read(SocketWrapper&) {}
void ServerShutdownMsg::write(SocketWrapper&) const {}