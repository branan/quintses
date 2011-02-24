#ifndef QNT_CLIENT_OPENGL_GLRENDER_HPP
#define QNT_CLIENT_OPENGL_GLRENDER_HPP

#include <map>

#include "core/renderiface.hpp"

class LoaderIface;

struct GlDrawable;

class GlRender : public RenderIface {
public:
  GlRender(LoaderIface*);
  virtual ~GlRender();

  virtual void pushMessage(ClientMsg*);
  virtual void draw() const;
private:
  LoaderIface* m_loader;
  std::map<uint32_t, GlDrawable*> m_drawables;
};

#endif // QNT_CLIENT_OPENGL_GLRENDER_HPP
