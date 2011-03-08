#ifndef QNT_CORE_LOADERIFACE_H
#define QNT_CORE_LOADERIFACE_H

/// \brief An asynchronous loader for the renderer
/// \ingroup Graphics
/// Subclasses of this must be written specifically for each
/// subclass of RenderIface.
class LoaderIface {
public:
  virtual ~LoaderIface();

  /// \brief End the thread
  /// This signals the loader thread to finish, and waits for confirmation
  virtual void finish() =0;
};

#endif // QNT_CORE_LOADERIFACE_H
