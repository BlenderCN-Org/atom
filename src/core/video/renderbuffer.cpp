#include "core/video/renderbuffer.h"

#include "core/video/video_service.h"
#include "core/video/texture.h"

namespace atom {

Renderbuffer::Renderbuffer(
  VideoService &vs,
  PixelFormat format,
  unsigned width,
  unsigned height)
  : my_gl_renderbuffer(0)
  , my_format(format)
  , my_width(width)
  , my_height(height)
{
  GL_ERROR_GUARD;
  glGenRenderbuffers(1, &my_gl_renderbuffer);

  vs.bind_renderbuffer(*this);
  glRenderbufferStorage(GL_RENDERBUFFER, Texture::pixel_format_to_gl_format(format), width, height);
  vs.unbind_renderbuffer();
}

Renderbuffer::~Renderbuffer()
{
  GL_ERROR_GUARD;
  glDeleteRenderbuffers(1, &my_gl_renderbuffer);
}

}
