#include "inputmsg.hpp"
#include "loadplayermsg.hpp"
#include "shutdownmsg.hpp"

ServerMsg::ServerMsg(ClientIface* sender) : m_sender(sender) {}

ServerMsg::~ServerMsg() {}

ServerMsg* ServerMsg::create(uint32_t type, ClientIface* cli) {
  switch(type) {
    case ShutdownMessage:
      return new ServerShutdownMsg(cli);
    case InputMessage:
      return new ServerInputMsg(cli);
    case LoadPlayer:
      return new ServerLoadPlayerMsg(cli);
  }
  return 0;
}
