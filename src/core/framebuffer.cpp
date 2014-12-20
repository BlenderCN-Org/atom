#include "framebuffer.h"
#include "texture.h"
#include "renderbuffer.h"

namespace atom {

Framebuffer::Framebuffer(VideoService &vs)
  : my_vs(vs)
  , my_gl_framebuffer(0)
  , my_color_attachments(0)
{
  GL_ERROR_GUARD;
  glGenFramebuffers(1, &my_gl_framebuffer);
}

Framebuffer::~Framebuffer()
{
  GL_ERROR_GUARD;
  glDeleteFramebuffers(1, &my_gl_framebuffer);
}

void Framebuffer::set_color_attachment(unsigned index, Texture &texture)
{
  GL_ERROR_GUARD;
  assert(index < 16); // momentalne je podporovanych len 16 attachmentov
                      // dobre by bolo zistit max pocet cez glGet
  assert(texture.type() == TextureType::TEXTURE_2D);
  my_vs.bind_write_framebuffer(*this, false);
  GLenum attachment = GL_COLOR_ATTACHMENT0 + index;
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachment, GL_TEXTURE_2D,
                         texture.gl_texture(), 0);
  my_color_attachments |= 1 << index;
}

void Framebuffer::set_color_attachment(unsigned index, Renderbuffer &renderbuffer)
{
  GL_ERROR_GUARD;
  assert(index < 16); // momentalne je podporovanych len 16 attachmentov
                      // dobre by bolo zistit max pocet cez glGet
//  assert(texture.type() == TextureType::TEXTURE_2D);
  my_vs.bind_write_framebuffer(*this, false);
  GLenum attachment = GL_COLOR_ATTACHMENT0 + index;
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, attachment, GL_RENDERBUFFER,
    renderbuffer.gl_renderbuffer());
  my_color_attachments |= 1 << index;
}

void Framebuffer::set_depth_attachment(Texture &texture)
{
  assert(texture.type() == TextureType::TEXTURE_2D);
  assert(texture.format() == PixelFormat::D16);
  GL_ERROR_GUARD;

  my_vs.bind_write_framebuffer(*this, false);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         texture.gl_texture(), 0);
}

void Framebuffer::set_depth_stencil_attachment(Renderbuffer &renderbuffer)
{
  assert(renderbuffer.format() == PixelFormat::D24S8);  // zatial len depth 24b stencil 8b
  GL_ERROR_GUARD;

  my_vs.bind_write_framebuffer(*this, false);
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                            renderbuffer.gl_renderbuffer());
}

bool Framebuffer::is_valid()
{
  GL_ERROR_GUARD;
  my_vs.unbind_write_framebuffer();
  my_vs.bind_write_framebuffer(*this, false);

  GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  return status == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::set_depth_stencil_attachment(Texture &texture)
{
  assert(texture.format() == PixelFormat::D24S8);  // zatial len depth 24b stencil 8b
  GL_ERROR_GUARD;
  my_vs.bind_write_framebuffer(*this, false);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D,
    texture.gl_texture(), 0);
}

}
