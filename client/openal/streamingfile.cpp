#include "streamingfile.hpp"

#include <al.h>

StreamingFile::StreamingFile(const char* fname) : m_next_buffer(0) {
  m_file.open(fname, std::fstream::binary | std::fstream::in);
  alGenBuffers(2, m_albuffers);
  alGenSources(1, &m_alsource);
}

StreamingFile::~StreamingFile() {
  alDeleteSources(1, &m_alsource);
  alDeleteBuffers(2, m_albuffers);
}

void StreamingFile::play() {
  readHeaders();
  readyNextBuffer();
  readyNextBuffer();
  alSourceQueueBuffers(m_alsource, 2, m_albuffers);
  alSourcePlay(m_alsource);
}

bool StreamingFile::update() {
  ALint val;
  bool result = true;
  static bool previous_result = true;
  alGetSourcei(m_alsource, AL_BUFFERS_PROCESSED, &val);
  if(val) {
    ALuint nextbuf = m_albuffers[m_next_buffer];
    alSourceUnqueueBuffers(m_alsource, 1, &nextbuf);
    bool new_result = readyNextBuffer();
    if(new_result)
      alSourceQueueBuffers(m_alsource, 1, &nextbuf);
    if((!new_result) && (!previous_result))
      result = false;
    else
      previous_result = new_result;
  }
  return result;
}

void StreamingFile::fillNextBuffer(void* data, size_t len) {
  if(!data) return; // TODO: mark this audio as completed so it can be cleaned up
  alBufferData(m_albuffers[m_next_buffer], m_format, data, len, m_frequency);
  m_next_buffer ^= 1; // toggle between 0 and 1
}
