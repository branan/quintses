#include "clientaddobjectmsg.hpp"

#include <iostream>

ClientAddObjectMsg::~ClientAddObjectMsg() {}
ClientAddDrawableMsg::~ClientAddDrawableMsg() {}
ClientAddAudibleMsg::~ClientAddAudibleMsg() {}

void ClientAddObjectMsg::read(std::iostream& stream) {
  stream.read(&m_objid, 4);
}

void ClientAddObjectMsg::write(std::iostream& stream) const {
  stream.write(&m_objid, 4);
}

ClientMsg::MessageType ClientAddDrawableMsg::type() const {
  return ClientMsg::AddDrawable;
}

ClientMsg::MessageType ClientAddAudibleMsg::type() const {
  return ClientMsg::AddAudible;
}
