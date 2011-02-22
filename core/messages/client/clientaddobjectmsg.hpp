#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP

#include "clientmsg.hpp"

#include <string>

struct ClientAddObjectParams {
  uint32_t m_objid;
  std::string m_template;

  ClientAddObjectParams(const std::string& t="", uint32_t id=0)
    : m_objid(id), m_template(t) {}
};

class ClientAddObjectMsg : public ClientMsg {
public:
  ClientAddObjectMsg(const ClientAddObjectParams& p) : m_objid(p.m_objid), m_template(p.m_template) {}
  virtual ~ClientAddObjectMsg();
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  uint32_t m_objid;
  std::string m_template;
};

class ClientAddDrawableMsg : public ClientAddObjectMsg {
public:
  ClientAddDrawableMsg(const ClientAddObjectParams& p) : ClientAddObjectMsg(p) {}
  virtual ~ClientAddDrawableMsg();
  virtual MessageType type() const;
};

class ClientAddAudibleMsg : public ClientAddObjectMsg {
public:
  ClientAddAudibleMsg(const ClientAddObjectParams& p) : ClientAddObjectMsg(p) {}
  virtual ~ClientAddAudibleMsg();
  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
