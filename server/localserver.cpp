#include "localserver.hpp"
#include "core/clientiface.hpp"

#include "bullet/btphysics.hpp"

#include "core/messages/server/inputmsg.hpp"
#include "core/messages/server/loadplayermsg.hpp"
#include "core/messages/server/shutdownmsg.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"
#include "core/messages/client/clientdelobjectmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"
#include "core/util/atomic.hpp"
#include "core/util/math.hpp"
#include "core/util/xml_helpers.hpp"

#include <fstream>
#include <iterator>
#include <iostream>

namespace {
  struct ServerLauncher {
    void operator()(LocalServer* s) {
      s->run();
    }
  };
}

LocalServer::LocalServer() : m_next_id(1), m_ticks(0) {
  m_running = false;
  ServerLauncher launcher;
  boost::thread t(launcher, this);
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(!m_running) {
    m_status_cond.wait(lock);
  }
  // server thread is on its own at this point
}

void LocalServer::run() {
  m_physics = new BtPhysics(this);
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = true;
  }
  m_status_cond.notify_all();

  loadWorld();

  bool looping = true;
  while(looping) {
    ServerMsg* msg;
    msg = m_msg_queue.wait_pop();
    switch(msg->type()) {
      case ServerMsg::ShutdownMessage:
        looping = false;
        delete msg;
        break;
      case ServerMsg::InputMessage:
        m_physics->pushMessage(msg);
        break;
      case ServerMsg::LoadPlayer:
        loadPlayer(static_cast<ServerLoadPlayerMsg*>(msg));
        delete msg;
        break;
      default:
        delete msg;
        break;
    }
  }

  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    i->first->serverClosed();
  }
  for(auto i = m_lobby_clients.begin(); i != m_lobby_clients.end(); ++i) {
    (*i)->serverClosed();
  }
  m_physics->finish();
  delete m_physics;
  {
    boost::mutex::scoped_lock lock(m_status_mutex);
    m_running = false;
  }
  m_status_cond.notify_all();
}

void LocalServer::addClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);

  // Add player drawables to all clients
  // locations will be synched on next physics tick
  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    // TODO: get template name from object info
    ClientAddObjectParams ap_old("Player", i->second);
    c->pushMessage(new ClientAddDrawableMsg(ap_old));
  }
  for(auto i = m_baddies.begin(); i != m_baddies.end(); ++i) {
    ClientAddObjectParams ab(i->m_templ, i->m_id);
    c->pushMessage(new ClientAddDrawableMsg(ab));
  }
  m_lobby_clients.insert(c);
}

void LocalServer::removeClient(ClientIface* c) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  uint32_t client_id = m_clients[c];
  m_clients.erase(c);
  m_lobby_clients.erase(c);
  if(client_id) {
    m_physics->delPhysical(client_id, true);
    for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
      i->first->pushMessage(new ClientDelDrawableMsg(client_id));
    }
    for(auto i = m_lobby_clients.begin(); i != m_lobby_clients.end(); ++i) {
      (*i)->pushMessage(new ClientDelDrawableMsg(client_id));
    }
  }
}

// TODO: privileged clients. For now any client can make any call
void LocalServer::makeClientPrivileged(ClientIface* ) { }

bool LocalServer::isLocal() const {
  return true;
}

void LocalServer::pushMessage(ServerMsg* msg) {
  m_msg_queue.push(msg);
}

int LocalServer::waitForTermination() const {
  boost::mutex::scoped_lock lock(m_status_mutex);
  while(m_running) {
    m_status_cond.wait(lock);
  }
  return 0;
}

void LocalServer::pushClientTransforms(LocalServer::ClientTransList& trans) {
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  for(auto i = trans.begin(); i != trans.end(); ++i) {
    ClientTransObjectParams tp;
    tp.m_transform = i->second;
    tp.m_objid = i->first;
    for(auto j = m_clients.begin(); j != m_clients.end(); ++j)
      j->first->pushMessage(new ClientTransDrawableMsg(tp));
    for(auto j = m_lobby_clients.begin(); j != m_lobby_clients.end(); ++j)
      (*j)->pushMessage(new ClientTransDrawableMsg(tp));
    delete[] i->second;
  }
}

void LocalServer::runTickCallbacks() {
  m_ticks++;
  if(m_ticks == 40)
    m_ticks = 0;
  uint16_t state;
  if(m_ticks < 20)
    state = ServerInputMsg::StrafeLeft;
  else
    state = ServerInputMsg::StrafeRight;
  for(auto i = m_baddies.begin(); i != m_baddies.end(); ++i) {
    m_physics->setInputStateCallback(i->m_id, state);
  }
}


uint32_t LocalServer::getNextIdentifier() {
  return fetchAndIncrement(m_next_id);
}

void LocalServer::loadPlayer(ServerLoadPlayerMsg *msg) {
  //TODO: validate the template passed here is OK for use as a player
  boost::lock_guard<boost::shared_mutex> lock(m_clients_mutex);
  ClientIface *client = msg->m_sender;
  m_lobby_clients.erase(client);
  uint32_t id = getNextIdentifier();
  m_physics->addPhysical(id, msg->m_template, glm::value_ptr(m_player_spawn), true);
  m_clients.insert(std::make_pair(client, id));
  ClientAddObjectParams p(msg->m_template, id);
  for(auto i = m_clients.begin(); i != m_clients.end(); ++i) {
    i->first->pushMessage(new ClientAddDrawableMsg(p));
  }
}

void LocalServer::loadWorld() {
  std::ifstream xml_file("data/world.xml");
  if(!xml_file) {
    //TODO: throw an error
    return;
  }
  std::string xml_data;
  std::copy(std::istream_iterator<char>(xml_file), std::istream_iterator<char>(), xml_data.begin());
  XmlDom xml(xml_data);
  // cleanup the file and string now that they're no longer needed
  xml_data = "";
  xml_file.close();
  rapidxml::xml_node<> *root_node = xml.doc()->first_node("QuintsesWorld");
  if(!root_node) {
    //TODO throw an error and disconnect all clients
    return;
  }
  rapidxml::xml_node<> *spawn_node = root_node->first_node("PlayerSpawn");
  if(spawn_node) {
    m_player_spawn = parseTransform(spawn_node->first_node("Transform"));
  }
  loadMobGroup(root_node->first_node("MobGroup")); // TODO: add support for more than one mob group
  for(rapidxml::xml_node<> *node = root_node->first_node("Blocker"); node != 0; node = node->next_sibling()) {
    char *templ = node->first_node("Template")->value();
    glm::mat4 trans = parseTransform(node->first_node("Transform"));
    m_physics->addPhysical(getNextIdentifier(), templ, glm::value_ptr(trans));
  }
}

void LocalServer::loadMobGroup(rapidxml::xml_node<> *node) {
  BaddieData b;
  b.m_templ = node->first_node("Template")->value();
  glm::quat quat = parseQuaternion(node->first_node("Quaternion"));
  float height = atof(node->first_node("Height")->value());
  rapidxml::xml_node<> *grid_node = node->first_node("Grid"); // TODO: support other sorts of mob layouts
  float xmin, xmax, xdelta;
  float ymin, ymax, ydelta;
  int xdiv, ydiv;
  xmin = atof(grid_node->first_node("xmin")->value());
  xmax = atof(grid_node->first_node("xmax")->value());
  ymin = atof(grid_node->first_node("ymin")->value());
  ymax = atof(grid_node->first_node("ymax")->value());
  xdiv = atoi(grid_node->first_node("xdiv")->value());
  ydiv = atoi(grid_node->first_node("ydiv")->value());
  xdelta = (xmax-xmin) / float(xdiv-1);
  ydelta = (ymax-ymin) / float(ydiv-1);
  for(float x = xmin, i = 0; i < xdiv; ++i, x += xdelta) {
    for(float y = ymin, j = 0; j < ydiv; ++j, y += ydelta) {
      b.m_id = getNextIdentifier();
      glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(x, y, height)) * glm::mat4_cast(quat);
      m_physics->addPhysical(b.m_id, b.m_templ, glm::value_ptr(trans), true); // monsters are characters too!
      m_baddies.push_back(b);
    }
  }
}
