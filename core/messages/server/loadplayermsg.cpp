#include "loadplayermsg.hpp"
#include "core/util/socket.hpp"

ServerLoadPlayerMsg::~ServerLoadPlayerMsg() {}

ServerMsg::MessageType ServerLoadPlayerMsg::type() const {
  return LoadPlayer;
}

void ServerLoadPlayerMsg::read(SocketWrapper& stream) {
  stream.readString(m_template);
}

void ServerLoadPlayerMsg::write(SocketWrapper& stream) const {
  stream.writeString(m_template);
}
