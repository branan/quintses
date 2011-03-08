#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP

#include <boost/cstdint.hpp>

class SocketWrapper;

class ClientMsg {
public:
  enum MessageType {
    AddDrawable,
    AddAudible,
    TransDrawable,
    TransAudible,
    DelDrawable,
    DelAudible,
  };

  virtual ~ClientMsg();
  virtual MessageType type() const =0;
  virtual void read(SocketWrapper &) =0;
  virtual void write(SocketWrapper &) const =0;

  static ClientMsg *create(uint32_t);
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP