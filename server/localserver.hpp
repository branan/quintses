#ifndef QNT_SERVER_LOCALSERVER_H
#define QNT_SERVER_LOCALSERVER_H

#include <map>
#include <vector>
#include <set>

#include <boost/thread.hpp>

#include "core/serveriface.hpp"
#include "core/util/queue.hpp"
#include "core/util/xml.hpp"
#include "glm/glm.hpp"

class PhysicsIface;
class ServerLoadPlayerMsg;

/// \brief The primary server class
/// \ingroup Server
/// This is a working server class that handles physics, AI, and
/// general game logic.
class LocalServer : public ServerIface {
public:
  LocalServer();

  void run();

  virtual void addClient(ClientIface*);
  virtual void removeClient(ClientIface*);
  virtual void makeClientPrivileged(ClientIface*);
  virtual bool isLocal() const;

  virtual void pushMessage(ServerMsg*);

  virtual int waitForTermination() const;

  /// \brief List of entity IDs and transformation matrices
  typedef std::vector<std::pair<uint32_t,float*> > ClientTransList;

  /// \brief Get the identifier for a given client pointer
  inline uint32_t getClientId(ClientIface* cli) { boost::shared_lock<boost::shared_mutex> lock(m_clients_mutex); return m_clients[cli]; }

  /// \brief Push object transformations to all clients
  /// This function locks the client list mutex for the duration of its execution
  void pushClientTransforms(ClientTransList&);

  /// \brief Run all game tick callbacks
  /// This function should be called by the physics implementation. It runs all game
  /// callbacks that should be executed each physics tick. This may include AI or animation messages.
  void runTickCallbacks();

private:
  std::map<ClientIface*, uint32_t> m_clients;
  std::set<ClientIface*> m_lobby_clients;
  glm::mat4 m_player_spawn;
  uint32_t m_next_id; // this is the global identifier counter. Hopefully we never have more than ~4 Billion objects

  struct BaddieData {
    std::string m_templ;
    uint32_t m_id;
  };
  std::vector<BaddieData> m_baddies;

  // event queue
  queue<ServerMsg*> m_msg_queue;

  // worker threads/subsystems
  PhysicsIface* m_physics;

  // thread management members
  bool m_running;
  mutable boost::condition_variable m_status_cond;
  mutable boost::mutex m_status_mutex;
  boost::shared_mutex m_clients_mutex; // read/write lock for clients list

  uint32_t getNextIdentifier();

  /// \brief Initialize player structures
  /// \internal
  /// This is a helper function to initialize a player character from a 
  void loadPlayer(ServerLoadPlayerMsg*);

  /// \brief Load the world XML
  /// \internal
  /// This function parses the world XML and initializes all startup entities
  void loadWorld();

  /// \brief Load a group of enemies
  /// \internal
  /// This parses a mob group XML node. Mob groups can be defined to be created
  /// based on different layouts and parameters. See the world XML reference for
  /// details.
  /// \note Currently only a uniform grid is supported
  void loadMobGroup(rapidxml::xml_node<>*);

  uint m_ticks;
};

#endif // QNT_SERVER_LOCALSERVER_H
