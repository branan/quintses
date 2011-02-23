#include "clientdelobjectmsg.hpp"

#include <iostream>

ClientDelObjectMsg::~ClientDelObjectMsg() {}
ClientDelDrawableMsg::~ClientDelDrawableMsg() {}
ClientDelAudibleMsg::~ClientDelAudibleMsg() {}

void ClientDelObjectMsg::read(std::iostream& stream) {
  stream.read((char*)&m_objid, 4);
}

void ClientDelObjectMsg::write(std::iostream& stream) const {
  stream.write((char*)&m_objid, 4);
}

ClientMsg::MessageType ClientDelDrawableMsg::type() const {
  return ClientMsg::DelDrawable;
}

ClientMsg::MessageType ClientDelAudibleMsg::type() const {
  return ClientMsg::DelAudible;
}
