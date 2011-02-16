#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP

#include "clientmsg.hpp"
#include <string.h>

class ClientTransObjectMsg : public ClientMsg {
public:
  ClientTransObjectMsg(float* mat) { memcpy(m_transform, mat, 64); }
  virtual ~ClientTransObjectMsg();
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  float m_transform[16];
};

class ClientTransDrawableMsg : public ClientTransObjectMsg {
public:
  ClientTransDrawableMsg(float* mat) : ClientTransObjectMsg(mat) {}
  virtual ~ClientTransDrawableMsg();
  virtual MessageType type() const;
};

class ClientTransAudibleMsg : public ClientTransObjectMsg {
public:
  ClientTransAudibleMsg(float* mat) : ClientTransObjectMsg(mat) {}
  virtual ~ClientTransAudibleMsg();
  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTTRANSOBJECTMSG_HPP