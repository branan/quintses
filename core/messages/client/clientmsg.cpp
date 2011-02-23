#include "clientmsg.hpp"
#include "clientaddobjectmsg.hpp"
#include "clientdelobjectmsg.hpp"
#include "clienttransobjectmsg.hpp"

ClientMsg::~ClientMsg() {}

ClientMsg* ClientMsg::create(uint32_t type) {
  switch(type) {
    case AddAudible:
      return new ClientAddAudibleMsg(ClientAddObjectParams());
    case AddDrawable:
      return new ClientAddDrawableMsg(ClientAddObjectParams());
    case TransAudible:
      return new ClientTransAudibleMsg(ClientTransObjectParams());
    case TransDrawable:
      return new ClientTransDrawableMsg(ClientTransObjectParams());
    case DelAudible:
      return new ClientDelAudibleMsg(0);
    case DelDrawable:
      return new ClientDelDrawableMsg(0);
  }
  return 0;
}
