#pragma once

#include "gl_utils.h"

namespace atom {

enum class ShaderType {
  VERTEX,
  GEOMETRY,
  PIXEL
};

class Shader : NonCopyable {
  ShaderType  my_type;
  bool        my_is_compiled;
  GLuint      my_gl_shader;

public:
  explicit Shader(ShaderType type);
  ~Shader();

  bool compile(const String &src);

  bool is_compiled() const;

  GLuint gl_shader() const;

  ShaderType get_shader_type() const
  {
    return my_type;
  }

private:
  static GLenum get_gl_shader_type(ShaderType type);
};

}
