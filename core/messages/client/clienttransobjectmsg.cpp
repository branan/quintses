#include "clienttransobjectmsg.hpp"

#include <iostream>

ClientTransObjectMsg::~ClientTransObjectMsg() {}
ClientTransDrawableMsg::~ClientTransDrawableMsg() {}
ClientTransAudibleMsg::~ClientTransAudibleMsg() {}

void ClientTransObjectMsg::read(std::iostream& stream) {
  stream.read((char*)&m_objid, 4);
  stream.read((char*)m_transform, 64);
}

void ClientTransObjectMsg::write(std::iostream& stream) const {
  stream.write((char*)&m_objid, 4);
  stream.write((char*)m_transform, 64);
}

ClientMsg::MessageType ClientTransDrawableMsg::type() const {
  return ClientMsg::TransDrawable;
}

ClientMsg::MessageType ClientTransAudibleMsg::type() const {
  return ClientMsg::TransAudible;
}
