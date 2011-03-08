#include "clientaddobjectmsg.hpp"

#include "core/util/socket.hpp"

ClientAddObjectMsg::~ClientAddObjectMsg() {}
ClientAddDrawableMsg::~ClientAddDrawableMsg() {}
ClientAddAudibleMsg::~ClientAddAudibleMsg() {}

void ClientAddObjectMsg::read(SocketWrapper &stream) {
  stream.readPod(m_objid);
  stream.readString(m_template);
}

void ClientAddObjectMsg::write(SocketWrapper &stream) const {
  stream.writePod(m_objid);
  stream.writeString(m_template);
}

ClientMsg::MessageType ClientAddDrawableMsg::type() const {
  return ClientMsg::AddDrawable;
}

ClientMsg::MessageType ClientAddAudibleMsg::type() const {
  return ClientMsg::AddAudible;
}
