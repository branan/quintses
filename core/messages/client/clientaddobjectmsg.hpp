#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP

#include "clientmsg.hpp"

#include <string>

struct ClientAddObjectParams {
  unsigned int m_objid;
  std::string m_template;
};

class ClientAddObjectMsg : public ClientMsg {
public:
  ClientAddObjectMsg(unsigned int id, const std::string& tmp) : m_objid(id), m_template(tmp) {}
  ClientAddObjectMsg(const ClientAddObjectParams& p) : m_objid(p.m_objid), m_template(p.m_template) {}
  virtual ~ClientAddObjectMsg();
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  unsigned int m_objid;
  std::string m_template;
};

class ClientAddDrawableMsg : public ClientAddObjectMsg {
public:
  ClientAddDrawableMsg(unsigned int id, const std::string& tmp) : ClientAddObjectMsg(id, tmp) {}
  ClientAddDrawableMsg(const ClientAddObjectParams& p) : ClientAddObjectMsg(p) {}
  virtual ~ClientAddDrawableMsg();
  virtual MessageType type() const;
};

class ClientAddAudibleMsg : public ClientAddObjectMsg {
public:
  ClientAddAudibleMsg(unsigned int id, const std::string& tmp) : ClientAddObjectMsg(id, tmp) {}
  ClientAddAudibleMsg(const ClientAddObjectParams& p) : ClientAddObjectMsg(p) {}
  virtual ~ClientAddAudibleMsg();
  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
