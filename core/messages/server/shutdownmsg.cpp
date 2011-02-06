#include "shutdownmsg.hpp"

ServerShutdownMsg::~ServerShutdownMsg() {}

ServerMessage::MessageType ServerShutdownMsg::type() const {
  return ServerMessage::ShutdownMessage;
}
