#pragma once

#include <GL/glew.h>
#include "core/noncopyable.h"
#include "core/corefwd.h"

namespace atom {

enum class VideoShaderType {
  VERTEX,
  GEOMETRY,
  PIXEL
};

class Shader : NonCopyable {
public:
  Shader(VideoService &vs, VideoShaderType type);
  ~Shader();

  bool compile(const String &src);

  bool is_compiled() const;

  GLuint gl_shader() const;

private:
  static GLenum get_gl_shader_type(VideoShaderType type);

private:
  VideoService    &my_vs;
  VideoShaderType  my_type;
  bool             my_is_compiled;
  GLuint           my_gl_shader;
};

}
