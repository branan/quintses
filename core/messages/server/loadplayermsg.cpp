#include "loadplayermsg.hpp"
#include "core/util/streamhelpers.hpp"

ServerLoadPlayerMsg::~ServerLoadPlayerMsg() {}

ServerMsg::MessageType ServerLoadPlayerMsg::type() const {
  return LoadPlayer;
}

void ServerLoadPlayerMsg::read(std::iostream& stream) {
  m_template = readString(stream);
}

void ServerLoadPlayerMsg::write(std::iostream& stream) const {
  writeString(stream, m_template);
}
