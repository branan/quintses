#ifndef QNT_CLIENT_OPENAL_STREAMINGFILE_HPP
#define QNT_CLIENT_OPENAL_STREAMINGFILE_HPP

#include <fstream>

/// \brief An OpenAL buffer streamed from a file
/// \ingroup Client
/// \ingroup Audio
/// This class wraps an OpenAL source and the buffers tied to it.
class StreamingFile {
public:
  /// \brief Initialize a StreamingFile
  /// \param filename The file to open and stream
  StreamingFile(const char* filename);
  virtual ~StreamingFile();

  /// \brief Start playing the file
  void play();
  
  /// \brief Update the buffers
  /// If no update is needed, this function returns immediately
  /// \return False if there is no more data to play. Else true.
  bool update();
protected:
  /// \brief Define the size of the buffer
  /// For use by subclasses
  size_t bufferLen() const { return 1<<16; }

  /// \brief Load data into next available buffer
  /// For use by subclasses. Should be called by readyNextBuffer() implementation
  /// \sa readyNextBuffer()
  void fillNextBuffer(void*, size_t);

  /// \brief Set the format and frequency of the stream
  /// For use by subclasses. Should be called by readHeaders() implementation
  /// \sa readHeaders()
  void setStreamProperties(unsigned int format, unsigned int frequency) {
    m_format = format;
    m_frequency = frequency;
  }

  /// \brief Read data from file and call fill the next buffer
  /// This must be implemented by a subclass. If needed, streaming decompression or decryption should go here
  /// \sa fillNextBuffer()
  virtual bool readyNextBuffer() =0;

  /// \brief Read the headers from the audio file
  /// This must be implemented by a subclass. The headers parsed here are used to set the audio format properties
  /// \sa setStreamProperties()
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
