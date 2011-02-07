#include "streamingwav.hpp"

#include <string.h>
#include <stdexcept>

#include <al.h>

#pragma pack(push, 0)
namespace {
  const char* riffid = "RIFF";
  const char* waveid = "WAVE";
  const char* fmtid = "fmt ";
  const char* dataid = "data";
  struct RiffHeader {
    char blockid[4];
    unsigned int length;
  };
  struct WaveFormat {
    unsigned short format;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
  };
}
#pragma pack(pop)

StreamingWav::StreamingWav(const char* fname): StreamingFile(fname) {}
StreamingWav::~StreamingWav() {}

void StreamingWav::readHeaders() {
  RiffHeader riff;
  WaveFormat fmt;

  m_file.read((char*)&riff, sizeof(RiffHeader));
  if(strncmp(riff.blockid, riffid, 4))
    throw std::runtime_error("Not a RIFF file");

  char ftype[4];
  m_file.read(ftype, 4);
  if(strncmp(ftype, waveid, 4))
    throw std::runtime_error("Not a WAVE file");

  m_file.read((char*)&riff, sizeof(RiffHeader));
  if(strncmp(riff.blockid, fmtid, 4))
    throw std::runtime_error("Expected WAVE fmt block");

  m_file.read((char*)&fmt, sizeof(WaveFormat));
  if(fmt.format != 1)
    throw std::runtime_error("Only PCM WAVE files are supported");
  if(fmt.bitsPerSample != 8 && fmt.bitsPerSample != 16)
    throw std::runtime_error("Only bit sizes of 8 and 16 are supported");
  if(fmt.numChannels > 2)
    throw std::runtime_error("Only mono and stereo files are supported");

  if(fmt.bitsPerSample == 8 && fmt.numChannels == 1) {
    setStreamProperties(AL_FORMAT_MONO8, fmt.sampleRate);
  } else if(fmt.bitsPerSample == 8 && fmt.numChannels == 2) {
    setStreamProperties(AL_FORMAT_STEREO8, fmt.sampleRate);
  } else if(fmt.bitsPerSample == 16 && fmt.numChannels == 1) {
    setStreamProperties(AL_FORMAT_MONO16, fmt.sampleRate);
  } else if(fmt.bitsPerSample == 16 && fmt.numChannels == 2) {
    setStreamProperties(AL_FORMAT_STEREO16, fmt.sampleRate);
  }

  m_file.read((char*)&riff, sizeof(RiffHeader));
  if(strncmp(riff.blockid, dataid, 4))
    throw std::runtime_error("Expected WAVE data block");
  m_len = riff.length;
  m_pos = 0;
}

bool StreamingWav::readyNextBuffer() {
  size_t remaining = m_len - m_pos;
  size_t bufsize = bufferLen();
  if(!remaining)
    return false;

  if(bufsize > remaining)
    bufsize = remaining;
  m_pos += bufsize;
  char* data = new char[bufsize];
  m_file.read(data, bufsize);
  fillNextBuffer(data, bufsize);
  delete[] data;
  return true;
}
