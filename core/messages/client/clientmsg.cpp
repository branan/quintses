#include "clientmsg.hpp"
#include "clientaddobjectmsg.hpp"
#include "clienttransobjectmsg.hpp"

ClientMsg::~ClientMsg() {}

ClientMsg* ClientMsg::create(unsigned int type) {
  switch(type) {
    case AddAudible:
      return new ClientAddAudibleMsg(0);
    case AddDrawable:
      return new ClientAddDrawableMsg(0);
    case TransAudible:
      return new ClientTransAudibleMsg(0,0);
    case TransDrawable:
      return new ClientTransDrawableMsg(0,0);
  }
  return 0;
}
