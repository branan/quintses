#ifndef QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP
#define QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP

#include <iosfwd>
#include <boost/cstdint.hpp>

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
  virtual void read(std::iostream&) =0;
  virtual void write(std::iostream&) const =0;

  static ClientMsg *create(uint32_t);
};

#endif // QNT_CORE_MESSAGES_CLIENT_CLIENTMSG_HPP