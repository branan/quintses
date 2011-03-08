#ifndef QNT_CORE_PHYSICSIFACE_HPP
#define QNT_CORE_PHYSICSIFACE_HPP

#include <boost/cstdint.hpp>
#include <string>

class ServerMsg;

/// \brief Interface for physics
/// \defgroup Physics Physics Classes
/// Subclasses of this are expected to create a thread, or run asynchronously in some other fashion.
/// The physics tick should be 30Hz. Other server-side code may make assumptions as to this length of time.
class PhysicsIface {
public:
  virtual ~PhysicsIface();

  /// \brief Add a message to the event queue
  /// Subclasses should empty the event queue every physics tick.
  /// \param msg The message to add to the queue
  virtual void pushMessage(ServerMsg *msg) =0;

  /// \brief End the physics thread
  /// Signals the physics thread to be terminated, and waits for that termination to occur
  virtual void finish() =0;

  /// \brief Add a new physical object to the world
  /// \note This function may be implemented asynchronously
  /// \param id The object id associated with the new physical o bject
  /// \param templ The template that this object should be based off of
  /// \param mat The initial transform matrix for this object, in OpenGL-compatible format
  /// \param character Whether or not this is a character object. Defaults to false.
  virtual void addPhysical(uint32_t id, const std::string& templ, float *mat, bool character = false) =0;

  /// \brief Remove a physical object from the world
  /// \note This function may be implemented asynchronously
  /// \param id The object id associated with the physical object
  /// \param character Whether or not this object is a character
  virtual void delPhysical(uint32_t id, bool character = false) =0;

  /// \brief Set the input state of a character
  /// \note This function may be implemented asynchronously.
  /// \param id The object id of the character
  /// \param state The input state to set.
  /// \sa ServerInputMsg
  virtual void setInputState(uint32_t id, uint16_t state) =0;

  /// \brief Set the input state synchronously
  /// This version may be faster than setInputState on some platforms. It is guaranteed to be
  /// synchronous and directly modifies the physics world state.
  /// \note It is unsafe to call this function except from a physics callback.
  /// \param id The object id of the character
  /// \param state The input state to set.
  /// \sa ServerInputMsg
  virtual void setInputStateCallback(uint32_t id, uint16_t state) =0;
};

#endif // QNT_CORE_PHYSICSIFACE_HPP
