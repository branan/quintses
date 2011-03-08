#include "clientdelobjectmsg.hpp"

#include "core/util/socket.hpp"

ClientDelObjectMsg::~ClientDelObjectMsg() {}
ClientDelDrawableMsg::~ClientDelDrawableMsg() {}
ClientDelAudibleMsg::~ClientDelAudibleMsg() {}

void ClientDelObjectMsg::read(SocketWrapper &stream) {
  stream.readPod(m_objid);
}

void ClientDelObjectMsg::write(SocketWrapper &stream) const {
  stream.writePod(m_objid);
}

ClientMsg::MessageType ClientDelDrawableMsg::type() const {
  return ClientMsg::DelDrawable;
}

ClientMsg::MessageType ClientDelAudibleMsg::type() const {
  return ClientMsg::DelAudible;
}
