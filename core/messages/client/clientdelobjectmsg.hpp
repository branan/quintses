#ifndef QNT_CORE_MESSAGE_CLIENT_CLIENTDELOBJECTMSG_HPP
#define QNT_CORE_MESSAGE_CLIENT_CLIENTDELOBJECTMSG_HPP

#include "clientmsg.hpp"

class ClientDelObjectMsg : public ClientMsg {
public:
  ClientDelObjectMsg(uint32_t id) : m_objid(id) {}
  virtual ~ClientDelObjectMsg();
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  uint32_t m_objid;
};

class ClientDelDrawableMsg : public ClientDelObjectMsg {
public:
  ClientDelDrawableMsg(uint32_t id) : ClientDelObjectMsg(id) {}
  virtual ~ClientDelDrawableMsg();

  virtual MessageType type() const;
};

class ClientDelAudibleMsg : public ClientDelObjectMsg {
public:
  ClientDelAudibleMsg(uint32_t id) : ClientDelObjectMsg(id) {}
  virtual ~ClientDelAudibleMsg();

  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGE_CLIENT_CLIENTDELOBJECTMSG_HPP
