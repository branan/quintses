#include "clienttransobjectmsg.hpp"

#include "core/util/socket.hpp"

ClientTransObjectMsg::~ClientTransObjectMsg() {}
ClientTransDrawableMsg::~ClientTransDrawableMsg() {}
ClientTransAudibleMsg::~ClientTransAudibleMsg() {}

void ClientTransObjectMsg::read(SocketWrapper &stream) {
  stream.readPod(m_objid);
  stream.read(m_transform, 64);
}

void ClientTransObjectMsg::write(SocketWrapper &stream) const {
  stream.writePod(m_objid);
  stream.write(m_transform, 64);
}

ClientMsg::MessageType ClientTransDrawableMsg::type() const {
  return ClientMsg::TransDrawable;
}

ClientMsg::MessageType ClientTransAudibleMsg::type() const {
  return ClientMsg::TransAudible;
}
