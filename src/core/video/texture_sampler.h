#pragma once

#include <GL/glew.h>
#include "core/noncopyable.h"

namespace atom {

enum TextureWrap {
  REPEAT,
  CLAMP_TO_EDGE,
  CLAMP_TO_BORDER
};

enum TextureFilter {
  LINEAR,
  NEAREST
};


class TextureSampler : NonCopyable {
public:
  TextureSampler(TextureFilter filter, TextureWrap wrap);
  ~TextureSampler();

  TextureFilter texture_filter() const;

  TextureWrap texture_wrap() const;

  GLuint gl_sampler() const;

private:
  GLuint        my_sampler;
  TextureFilter my_filter;
  TextureWrap   my_wrap;
};

}
