#ifndef QNT_CLIENT_OPENAL_ALAUDIO_HPP
#define QNT_CLIENT_OPENAL_ALAUDIO_HPP

#include <set>

#include <boost/thread.hpp>

#include "core/audioiface.hpp"

struct ALCcontext_struct;
struct ALCdevice_struct;

class StreamingFile;

class AlAudio : public AudioIface {
public:
  AlAudio();
  virtual ~AlAudio();

//   virtual void playFile(const char*);

  virtual void finish();
  void run();

private:
  // thread management variables
  bool m_running, m_finish;
  boost::mutex m_status_mutex;
  boost::condition_variable m_status_cond;

  // OpenAL objects
  ALCdevice_struct* m_device;
  ALCcontext_struct* m_context;

  typedef std::set<StreamingFile*> StreamSet;
  StreamSet m_streams;
};

#endif // QNT_CLIENT_OPENAL_ALAUDIO_HPP