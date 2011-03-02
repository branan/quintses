#ifndef QNT_CORE_MESSAGES_SERVER_LOADPLAYERMSG_HPP
#define QNT_CORE_MESSAGES_SERVER_LOADPLAYERMSG_HPP

#include "servermsg.hpp"

#include <string>

class ServerLoadPlayerMsg : public ServerMsg {
public:
  ServerLoadPlayerMsg(ClientIface *cli) : ServerMsg(cli) {}
  ServerLoadPlayerMsg(ClientIface* cli, const std::string& tmp) : ServerMsg(cli), m_template(tmp) {}
  virtual ~ServerLoadPlayerMsg();

  virtual MessageType type() const;
  virtual void read(std::iostream&);
  virtual void write(std::iostream&) const;

  std::string m_template;
};

#endif // QNT_CORE_MESSAGES_SERVER_LOADPLAYERMSG_HPP