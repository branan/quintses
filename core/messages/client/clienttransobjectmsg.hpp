#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP

#include "clientmsg.hpp"
#include <string.h>

struct ClientTransObjectParams {
  uint32_t m_objid;
  float *m_transform;

  ClientTransObjectParams(float *trans = 0, uint32_t id = 0)
    : m_objid(id), m_transform(trans) {}
};

class ClientTransObjectMsg : public ClientMsg {
public:
  ClientTransObjectMsg(const ClientTransObjectParams& p) : m_objid(p.m_objid) { if(p.m_transform) memcpy(m_transform, p.m_transform, 64); }
  virtual ~ClientTransObjectMsg();
  virtual void read(SocketWrapper&);
  virtual void write(SocketWrapper&) const;

  uint32_t m_objid;
  float m_transform[16];
};

class ClientTransDrawableMsg : public ClientTransObjectMsg {
public:
  ClientTransDrawableMsg(const ClientTransObjectParams& p) : ClientTransObjectMsg(p) {}
  virtual ~ClientTransDrawableMsg();
  virtual MessageType type() const;
};

class ClientTransAudibleMsg : public ClientTransObjectMsg {
public:
  ClientTransAudibleMsg(const ClientTransObjectParams& p) : ClientTransObjectMsg(p) {}
  virtual ~ClientTransAudibleMsg();
  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP