#pragma once

#include "../utils/gl_utils.h"
#include "pixel.h"

namespace atom {

/**
 * Objekt podobny texture, ale optimalizovany ako ciel do Framebuffer-u. Neumoznuje citanie dat.
 * Ak potrebujeme data citat/resamplovat je nutne pouzit texturu.
 */
class Renderbuffer : private NonCopyable {
public:
  Renderbuffer(
    VideoService &vs,
    PixelFormat format,
    unsigned width,
    unsigned height);

  ~Renderbuffer();

  GLuint gl_renderbuffer() const
  { return my_gl_renderbuffer; }

  PixelFormat format() const
  { return my_format; }

  unsigned width() const
  { return my_width; }

  unsigned height() const
  { return my_height; }

private:
  GLuint my_gl_renderbuffer;
  PixelFormat my_format;
  unsigned my_width;
  unsigned my_height;
};

}
