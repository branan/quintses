#ifndef QNT_CORE_UTIL_SOCKET_HPP
#define QNT_CORE_UTIL_SOCKET_HPP

#include <string>
#include <boost/asio.hpp>

class SocketWrapper {
public:
  template <typename T>
  void readPod(T&val) {
    read(&val, sizeof(T));
  }
  template <typename T>
  void writePod(T val) {
    write(&val, sizeof(T));
  }

  void readString(std::string&str) {
    uint32_t len;
    readPod(len);
    char *buf = new char[len+1];
    buf[len] = 0;
    read(buf, len);
    str = buf;
    delete[] buf;
  }

  void writeString(std::string str) {
    uint32_t len = str.size();
    writePod(len);
    write(str.c_str(), len);
  }

  virtual void read(void *buf, size_t size) =0;
  virtual void write(const void *buf, size_t size) =0;
};

template <class Protocol>
class AsioSocket : public SocketWrapper {
  boost::asio::basic_stream_socket<Protocol> &m_stream;
public:
  AsioSocket(boost::asio::basic_stream_socket<Protocol> &stream) : m_stream(stream) {}

  virtual void read(void* buf, size_t size) {
    boost::asio::read(m_stream, boost::asio::buffer(buf, size));
  }

  virtual void write(const void* buf, size_t size) {
    boost::asio::write(m_stream, boost::asio::buffer(buf, size));
  }
};

#endif // QNT_CORE_UTIL_SOCKET_HPP
