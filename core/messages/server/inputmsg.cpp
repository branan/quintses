#include "inputmsg.hpp"

#include <iostream>

ServerInputMsg::~ServerInputMsg() {}

void ServerInputMsg::read(std::iostream& stream) {
  stream.read((char*)&m_state, 2);
}

void ServerInputMsg::write(std::iostream& stream) const {
  stream.write((char*)&m_state, 2);
}

ServerMsg::MessageType ServerInputMsg::type() const {
  return ServerMsg::InputMessage;
}
