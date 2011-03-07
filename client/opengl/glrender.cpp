#include "glew/glew.h"

#include "glrender.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"
#include "core/messages/client/clientdelobjectmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"

struct GlDrawable {
  float m_color[3];
  float m_matrix[16];
};

GlRender::GlRender(LoaderIface* ldr) : m_loader(ldr) {
  glewInit();
}

GlRender::~GlRender() {
  for(auto i = m_drawables.begin(); i != m_drawables.end(); ++i)
    delete i->second;
}

void GlRender::pushMessage(ClientMsg* cmsg) {
  switch(cmsg->type()) {
    case ClientMsg::AddDrawable: {
      ClientAddObjectMsg* msg = static_cast<ClientAddObjectMsg*>(cmsg);
      if(m_drawables.find(msg->m_objid) != m_drawables.end())
        return; // this object already exists
      GlDrawable *draw = new GlDrawable;
      if(msg->m_template == "Player") {
        draw->m_color[0]  = draw->m_color[2] = 0.f;
        draw->m_color[1] = 1.f;
      } else if(msg->m_template == "Baddie") {
        draw->m_color[1] = draw->m_color[2] = 0.f;
        draw->m_color[0] = 1.f;
      } else {
        draw->m_color[0] = draw->m_color[1] = draw->m_color[2] = 1.f;
      }
      m_drawables.insert(std::make_pair(msg->m_objid, draw));
      return;
    }
    case ClientMsg::TransDrawable: {
      ClientTransObjectMsg* msg = static_cast<ClientTransObjectMsg*>(cmsg);
      auto i = m_drawables.find(msg->m_objid);
      if(i == m_drawables.end())
        return; // This object doesn't exist
      memcpy(i->second->m_matrix, msg->m_transform, 64);
      return;
    }
    case ClientMsg::DelDrawable: {
      ClientDelObjectMsg* msg = static_cast<ClientDelObjectMsg*>(cmsg);
      m_drawables.erase(msg->m_objid);
      return;
    }
    default:
      return;
  }
}

void GlRender::draw() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  for(auto i = m_drawables.begin(); i != m_drawables.end(); ++i) {
    glLoadMatrixf(i->second->m_matrix);
    glColor3fv(i->second->m_color);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.f, 1.f, 0.f);
    glVertex3f(1.f, -1.f, 0.f);
    glVertex3f(-1.f, -1.f, 0.f);
    glEnd();
  }
}