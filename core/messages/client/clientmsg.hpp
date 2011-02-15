#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP

#include <iosfwd>

class ClientMsg {
public:
  enum MessageType {
    AddDrawable,
    AddAudible,
  };

  virtual ~ClientMsg();
  virtual MessageType type() const =0;
  virtual void read(std::iostream&) =0;
  virtual void write(std::iostream&) const =0;
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP