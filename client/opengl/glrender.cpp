#include "glew/glew.h"

#include "glrender.hpp"
#include "core/messages/client/clientaddobjectmsg.hpp"

class GlDrawable {
};

GlRender::GlRender(LoaderIface* ldr) : m_loader(ldr) {
  glewInit();
}

GlRender::~GlRender() {}

void GlRender::pushMessage(ClientMsg* cmsg) {
  switch(cmsg->type()) {
    case ClientMsg::AddDrawable: {
      ClientAddObjectMsg* msg = static_cast<ClientAddObjectMsg*>(cmsg);
      if(m_drawables.find(msg->m_objid) != m_drawables.end())
        return; // this object already exists
      m_drawables.insert(std::make_pair(msg->m_objid, new GlDrawable));
      return;
    }
    default:
      return;
  }
}

void GlRender::draw() const {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBegin(GL_TRIANGLES);
  glVertex3f(0.f, 1.f, -6.f);
  glVertex3f(1.f, -1.f, -6.f);
  glVertex3f(-1.f, -1.f, -6.f);
  glEnd();
}