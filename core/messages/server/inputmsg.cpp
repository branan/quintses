#include "inputmsg.hpp"

#include "core/util/socket.hpp"

ServerInputMsg::~ServerInputMsg() {}

void ServerInputMsg::read(SocketWrapper& stream) {
  stream.readPod(m_state);
}

void ServerInputMsg::write(SocketWrapper& stream) const {
  stream.writePod(m_state);
}

ServerMsg::MessageType ServerInputMsg::type() const {
  return ServerMsg::InputMessage;
}
