#include "shader.h"
#include "utils.h"
#include "gl_utils.h"
#include "log.h"

namespace atom {

Shader::Shader(ShaderType type)
  : my_type(type)
  , my_is_compiled(false)
  , my_gl_shader(0)
{
  my_gl_shader = glCreateShader(get_gl_shader_type(type));
}

Shader::~Shader()
{
  glDeleteShader(my_gl_shader);
}

bool Shader::compile(const String &src)
{
  GL_ERROR_GUARD;

  const char *source = src.c_str();
  glShaderSource(my_gl_shader, 1, &source, nullptr);
  glCompileShader(my_gl_shader);
  GLint status;
  glGetShaderiv(my_gl_shader, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    my_is_compiled = false;
    const int INFO_LENGTH = 2048;
    char compile_info[INFO_LENGTH];
    glGetShaderInfoLog(my_gl_shader, INFO_LENGTH, nullptr, compile_info);
    log_warning("Can't compile shader, status: %s", compile_info);
    return false;
  }

  my_is_compiled = true;
  return true;
}

bool Shader::is_compiled() const
{
  return my_is_compiled;
}

GLuint Shader::gl_shader() const
{
  return my_gl_shader;
}

GLenum Shader::get_gl_shader_type(ShaderType type)
{
  switch (type) {
    case ShaderType::VERTEX:
      return GL_VERTEX_SHADER;

    case ShaderType::GEOMETRY:
      return GL_GEOMETRY_SHADER;

    case ShaderType::PIXEL:
      return GL_FRAGMENT_SHADER;

    default:
      error("Invalid VideoShaderType");
      return GL_INVALID_ENUM;
  }
}

}
