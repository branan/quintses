#ifndef QNT_CORE_AUDIOIFACE_HPP
#define QNT_CORE_AUDIOIFACE_HPP

/// \brief Audio thread interface
/// \defgroup Audio Audio Classes
class AudioIface {
public:
  virtual ~AudioIface();

//   virtual void playFile(const char*) =0;

  virtual void finish() =0;
};

#endif // QNT_CORE_AUDIOIFACE_HPP