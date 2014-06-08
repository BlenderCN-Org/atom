#pragma once

#include <GL/glew.h>
#include "core/noncopyable.h"
#include "core/corefwd.h"
#include "core/stdvec.h"

namespace atom {

struct VideoUniform {
  String name;
  Type   type;
  GLint  location;
};

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

  static Type get_type_from_gl_type(GLenum type);

private:
  const VideoUniform* find_param(const char *name) const;

private:
  VideoService              &my_vs;
  GLuint                     my_gl_program;
  std::vector<VideoUniform>  my_uniforms;
};

}
