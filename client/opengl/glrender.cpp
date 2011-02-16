#include "glew/glew.h"

#include "glrender.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"
#include "core/messages/client/clienttransobjectmsg.hpp"

struct GlDrawable {
  float m_matrix[16];
};

GlRender::GlRender(LoaderIface* ldr) : m_loader(ldr) {
  glewInit();
}

GlRender::~GlRender() {
  for(DrawableMap::const_iterator i = m_drawables.begin(); i != m_drawables.end(); ++i)
    delete i->second;
}

void GlRender::pushMessage(ClientMsg* cmsg) {
  switch(cmsg->type()) {
    case ClientMsg::AddDrawable: {
      ClientAddObjectMsg* msg = static_cast<ClientAddObjectMsg*>(cmsg);
      if(m_drawables.find(msg->m_objid) != m_drawables.end())
        return; // this object already exists
      m_drawables.insert(std::make_pair(msg->m_objid, new GlDrawable));
      return;
    }
    case ClientMsg::TransDrawable: {
      ClientTransObjectMsg* msg = static_cast<ClientTransObjectMsg*>(cmsg);
      DrawableMap::iterator i = m_drawables.find(msg->m_objid);
      if(i == m_drawables.end())
        return; // This object doesn't exist
      memcpy(i->second->m_matrix, msg->m_transform, 64);
    }
    default:
      return;
  }
}

void GlRender::draw() const {
  for(DrawableMap::const_iterator i = m_drawables.begin(); i != m_drawables.end(); ++i) {
    glLoadMatrixf(i->second->m_matrix);
    glBegin(GL_TRIANGLES);
    glVertex3f(0.f, 1.f, 0.f);
    glVertex3f(1.f, -1.f, 0.f);
    glVertex3f(-1.f, -1.f, 0.f);
    glEnd();
  }
}