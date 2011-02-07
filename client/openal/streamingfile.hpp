#ifndef QNT_CLIENT_OPENAL_STREAMINGFILE_HPP
#define QNT_CLIENT_OPENAL_STREAMINGFILE_HPP

#include <fstream>

class StreamingFile {
public:
  StreamingFile(const char*);
  virtual ~StreamingFile();

  void play();
  bool update();
protected:
  size_t bufferLen() const { return 1<<16; }
  void fillNextBuffer(void*, size_t);
  void setStreamProperties(unsigned int format, unsigned int frequency) {
    m_format = format;
    m_frequency = frequency;
  }
  virtual bool readyNextBuffer() =0;
  virtual void readHeaders() =0;

  // OpenAL data
  unsigned int m_next_buffer;
  unsigned int m_albuffers[2];
  unsigned int m_alsource;
  unsigned int m_format;
  unsigned int m_frequency;

  std::ifstream m_file;
};

#endif // QNT_CLIENT_OPENAL_STREAMINGFILE_HPP
