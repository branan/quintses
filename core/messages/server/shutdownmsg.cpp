#include "shutdownmsg.hpp"

ServerShutdownMsg::ServerShutdownMsg(ClientIface* sender): ServerMessage(sender) {}
ServerShutdownMsg::~ServerShutdownMsg() {}

ServerMessage::MessageType ServerShutdownMsg::type() const {
  return ServerMessage::ShutdownMessage;
}

size_t ServerShutdownMsg::size() const {
  return sizeof(ServerShutdownMsg);
}
