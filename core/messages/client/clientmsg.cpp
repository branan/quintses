#include "clientmsg.hpp"
#include "clientaddobjectmsg.hpp"
#include "clienttransobjectmsg.hpp"

ClientMsg::~ClientMsg() {}

ClientMsg* ClientMsg::create(unsigned int type) {
  switch(type) {
    case AddAudible:
      return new ClientAddAudibleMsg(ClientAddObjectParams());
    case AddDrawable:
      return new ClientAddDrawableMsg(ClientAddObjectParams());
    case TransAudible:
      return new ClientTransAudibleMsg(ClientTransObjectParams());
    case TransDrawable:
      return new ClientTransDrawableMsg(ClientTransObjectParams());
  }
  return 0;
}
