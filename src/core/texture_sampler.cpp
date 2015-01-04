#include "texture_sampler.h"
#include "gl_utils.h"
#include "log.h"

namespace atom {

TextureSampler::TextureSampler(TextureFilter filter, TextureWrap wrap)
  : my_filter(filter)
  , my_wrap(wrap)
{
  GL_ERROR_GUARD;
  glGenSamplers(1, &my_sampler);

  switch (my_filter) {
    case TextureFilter::LINEAR:
      glSamplerParameteri(my_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glSamplerParameteri(my_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      break;

    case TextureFilter::NEAREST:
      glSamplerParameteri(my_sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glSamplerParameteri(my_sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      break;

    default:
      log_error("Unknown TextureFilter value");
      break;
  }

  switch (my_wrap) {
    case TextureWrap::REPEAT:
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
      break;

    case TextureWrap::CLAMP_TO_EDGE:
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      break;

    case TextureWrap::CLAMP_TO_BORDER:
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      glSamplerParameteri(my_sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      break;

    default:
      log_error("Unknown TextureWrap value");
      break;
  }
}

TextureSampler::~TextureSampler()
{
  glDeleteSamplers(1, &my_sampler);
}

TextureFilter TextureSampler::texture_filter() const
{
  return my_filter;
}

TextureWrap TextureSampler::texture_wrap() const
{
  return my_wrap;
}

GLuint TextureSampler::gl_sampler() const
{
  return my_sampler;
}

}
