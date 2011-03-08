#ifndef QNT_CLIENT_OPENAL_STREAMINGWAV_HPP
#define QNT_CLIENT_OPENAL_STREAMINGWAV_HPP

#include "streamingfile.hpp"

/// \brief A wav audio file streamed from the hard disk
/// \ingroup Client
/// \ingroup Audio
/// This class wraps uncompressed PCM audio in the wav format
class StreamingWav : public StreamingFile {
public:
  StreamingWav(const char*);
  virtual ~StreamingWav();

protected:
  virtual bool readyNextBuffer();
  virtual void readHeaders();

  // WAV file positioning
  size_t m_pos;
  size_t m_len;
};

#endif // QNT_CLIENT_OPENAL_STREAMINGWAV_HPP
