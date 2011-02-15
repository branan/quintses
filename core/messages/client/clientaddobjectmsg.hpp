#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP

#include "clientmsg.hpp"

class ClientAddObjectMsg : public ClientMsg {
public:
  virtual ~ClientAddObjectMsg();
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  unsigned int m_objid;
};

class ClientAddDrawableMsg : public ClientAddObjectMsg {
public:
  virtual ~ClientAddDrawableMsg();
  virtual MessageType type() const;
};

class ClientAddAudibleMsg : public ClientAddObjectMsg {
public:
  virtual ~ClientAddAudibleMsg();
  virtual MessageType type() const;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTADDOBJECTMSG_HPP
