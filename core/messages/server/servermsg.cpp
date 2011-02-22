#include "servermsg.hpp"
#include "shutdownmsg.hpp"

ServerMsg::ServerMsg(ClientIface* sender) : m_sender(sender) {}

ServerMsg::~ServerMsg() {}

ServerMsg* ServerMsg::create(uint32_t type, ClientIface* cli) {
  switch(type) {
    case ShutdownMessage:
      return new ServerShutdownMsg(cli);
  }
  return 0;
}
