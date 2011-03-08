#ifndef QNT_CORE_RENDERIFACE_HPP
#define QNT_CORE_RENDERIFACE_HPP

#include <string>

class ClientMsg;

/// \brief Displays the world to the client
/// \defgroup Grahpics Graphics Classes
/// Implementations of this class will always be used in the main
/// client thread. If asynchronous loading is desired, a subclass of
/// LoaderIface should be created to integrate with your implementation of
/// this class.
class RenderIface {
public:
  virtual ~RenderIface();

  /// \brief Read a message
  /// Since this class runs in the main thread, it has no event queue. This function
  /// is named the same as all other message-receive functions for consistency.
  virtual void pushMessage(ClientMsg*) =0;

  /// \brief Draw the scene
  /// This draws the scene to the client window. If the window is double-buffered, it is
  /// the responsibility of the client to swap the buffers.
  virtual void draw() const =0;
};

#endif // QNT_CORE_RENDERIFACE_HPP
