#pragma once

#include <GL/glew.h>
#include "../foundation.h"
#include "../stdvec.h"

namespace atom {

struct ShaderUniform {
  Type   type;
  String name;
  GLint  gl_location;
};

typedef std::vector<ShaderUniform> ShaderUniforms;

class Technique : NonCopyable {
  GLuint         my_gl_program;
  ShaderUniforms my_uniforms;

public:
  Technique();
  ~Technique();

  static uptr<Technique> create(const String &name);

  /**
   * Link shader programs. Then you should locate and map uniform.
   */
  bool link(const Shader &a, const Shader &b);
  bool link(const Shader &a, const Shader &b, const Shader &c);
  bool link(const Shader *shaders[], int count);

  void set_param(const char *name, const Vec3f &v) const;
  void set_param(const char *name, const Mat4f &m) const;

  GLuint gl_program() const;

  void locate_uniforms();

  void pull(const MetaObject &properties);

  static Type get_type_from_gl_type(GLenum type, GLint size);

  static bool get_shader_uniform_info(GLuint gl_program, GLuint index, ShaderUniform &uniform);

private:
  const ShaderUniform* find_param(const char *name) const;

  static bool load_and_compile(const String &filename, Shader &shader);
};

}
