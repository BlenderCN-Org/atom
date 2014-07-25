#pragma once

#include <GL/glew.h>
#include "../noncopyable.h"
#include "../corefwd.h"
#include "../stdvec.h"
#include "../math/mat.h"

namespace atom {

struct ShaderUniform {
  Type   type;
  String name;
  GLint  gl_location;
};

typedef std::vector<ShaderUniform> ShaderUniforms;

class Technique : NonCopyable {
public:
  Technique(VideoService &vs);
  ~Technique();

  static uptr<Technique> create(VideoService &vs, const String &name);

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

  static Type get_type_from_gl_type(GLenum type);

  static bool get_shader_uniform_info(GLuint gl_program, GLuint index, ShaderUniform &uniform);

private:
  const ShaderUniform* find_param(const char *name) const;

  static bool load_and_compile(const String &filename, Shader &shader);

private:
  VideoService  &my_vs;
  GLuint         my_gl_program;
  ShaderUniforms my_uniforms;
};

}
