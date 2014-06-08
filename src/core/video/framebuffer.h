#pragma once

#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/video/video_service.h"

namespace atom {

class Framebuffer : NonCopyable {
public:
  explicit Framebuffer(VideoService &vs);

  ~Framebuffer();

  /**
   * Nastavi color attachment na texturu. Neuvolnuje danu texturu v destruktore.
   *
   * Pripoji tento framebuffer ako draw framebuffer (aby mu mohol nastavit color attachment).
   * @todo premenovat na set_color_texture, set_color_renderbuffer???
   * @todo pouzit shared_ptr TexturePtr, RenderbufferPtr???
   */
  void set_color_attachment(unsigned index, Texture &texture);
  void set_color_attachment(unsigned index, Renderbuffer &renderbuffer);
  void set_depth_attachment(Texture &texture);
  void set_depth_attachment(Renderbuffer &renderbuffer);
//  void set_stencil_attachment(VideoService &vs, Texture &texture);
//  void set_stencil_attachment(VideoService &vs, Renderbuffer &renderbuffer);
  void set_depth_stencil_attachment(Texture &texture);
  void set_depth_stencil_attachment(Renderbuffer &renderbuffer);

//    bool is_complete();

  GLuint get_gl_framebuffer() const
  {
    return my_gl_framebuffer;
  }

  bool is_valid();

  /**
   * Vrat pripojene color attachmenty v bitovej maske.
   *
   * Napr. ak mam pripojeny color attachment 0 a 2, tak maska ma hodnotu 5.
   */
  unsigned color_attachments() const
  { return my_color_attachments; }

private:

//  GLenum get_gl_framebuffer_target(VideoService &vs)
//  {
//    if (vp.current_draw_framebuffer() == this) {
//      return GL_DRAW_FRAMEBUFFER;
//    } else if (vp.current_read_framebuffer() == this) {
//      return GL_READ_FRAMEBUFFER;
//    } else {
//      log::error("The framebuffer is not bound, can't set color attachment");
//      return GL_INVALID_ENUM;
//    }
//  }

private:
  VideoService &my_vs;
  GLuint        my_gl_framebuffer;
  unsigned      my_color_attachments;          ///< bitova maska color attachmentov
};

}
