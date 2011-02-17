#include "clientaddobjectmsg.hpp"

#include "core/util/streamhelpers.hpp"

#include <iostream>

ClientAddObjectMsg::~ClientAddObjectMsg() {}
ClientAddDrawableMsg::~ClientAddDrawableMsg() {}
ClientAddAudibleMsg::~ClientAddAudibleMsg() {}

void ClientAddObjectMsg::read(std::iostream& stream) {
  stream.read((char*)&m_objid, 4);
  m_template = readString(stream);
}

void ClientAddObjectMsg::write(std::iostream& stream) const {
  stream.write((char*)&m_objid, 4);
  writeString(stream, m_template);
}

ClientMsg::MessageType ClientAddDrawableMsg::type() const {
  return ClientMsg::AddDrawable;
}

ClientMsg::MessageType ClientAddAudibleMsg::type() const {
  return ClientMsg::AddAudible;
}
