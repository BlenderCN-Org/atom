#include "technique.h"
#include "core/meta.h"
#include "core/utils/utils.h"
#include "core/utils/gl_utils.h"
#include "core/video/shader.h"
#include "core/constants.h"

namespace atom {

Technique::Technique(VideoService &vs)
  : my_vs(vs)
{
  my_gl_program = glCreateProgram();
}

Technique::~Technique()
{
  glDeleteProgram(my_gl_program);
}

uptr<Technique> Technique::create(VideoService &vs, const String &name)
{
  Shader pixel_shader(vs, VideoShaderType::PIXEL);
  Shader vertex_shader(vs, VideoShaderType::VERTEX);
  Shader geometry_shader(vs, VideoShaderType::GEOMETRY);

  String pixel_src, vertex_src, geometry_src;
  const String prefix = String(SHADER_RESOURCE_DIR) + "/" + name;
  const String pixel_src_filename = prefix + ".ps";
  const String vertex_src_filename = prefix + ".vs";
  const String geometry_src_filename = prefix + ".gs";
  // load vertex shader
  if (!load_and_compile(vertex_src_filename, vertex_shader)) {
    log::warning("Can't load shader \"%s\"", vertex_src_filename.c_str());
    return nullptr;
  }
  // load pixel shader
  if (!load_and_compile(pixel_src_filename, pixel_shader)) {
    log::warning("Can't load shader \"%s\"", pixel_src_filename.c_str());
    return nullptr;
  }
  load_and_compile(geometry_src_filename, geometry_shader);

  const Shader *shaders[3] = { &vertex_shader, &pixel_shader, &geometry_shader };

  uptr<Technique> program(new Technique(vs));
  if (!program->link(shaders, geometry_shader.is_compiled() ? 3 : 2)) {
    log::warning("Can't link program \"%s\"", name.c_str());
    return nullptr;
  }

  program->locate_uniforms();
  return program;
}

bool Technique::link(const Shader &a, const Shader &b)
{
  const Shader *shaders[] = { &a, &b };
  return link(shaders, 2);
}

bool Technique::link(const Shader &a, const Shader &b, const Shader &c)
{
  const Shader *shaders[] = { &a, &b, &c };
  return link(shaders, 3);
}

bool Technique::link(const Shader *shaders[], int count)
{
  // check that all shaders are compiled
  for (int i = 0; i < count; ++i) {
    if (!shaders[i]->is_compiled()) {
      return false;
    }
  }

  for (int i = 0; i < count; ++i) {
    glAttachShader(my_gl_program, shaders[i]->gl_shader());
  }

  glLinkProgram(my_gl_program);

  for (int i = 0; i < count; ++i) {
    glDetachShader(my_gl_program, shaders[i]->gl_shader());
  }

  GLint status;
  glGetProgramiv(my_gl_program, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    const int INFO_LENGTH = 2048;
    char link_info[INFO_LENGTH];
    glGetProgramInfoLog(my_gl_program, INFO_LENGTH, nullptr, link_info);
    log::warning("Can't link shader program. Link info:\n%s\n", link_info);
    return false;
  }

  return true;
}

void Technique::set_param(const char *name, const Vec3f &v) const
{
  GL_ERROR_GUARD;
  const ShaderUniform *uniform = find_param(name);

  if (uniform != nullptr) {
    glUniform3f(uniform->gl_location, v.x, v.y, v.z);
  } else {
    log::warning("Uniform not found \"%s\"", name);
  }
}

void Technique::set_param(const char *name, const Mat4f &m) const
{
  GL_ERROR_GUARD;
  const ShaderUniform *uniform = find_param(name);

  if (uniform != nullptr) {
    glUniformMatrix4fv(uniform->gl_location, 1, false, reinterpret_cast<const GLfloat *>(&m));
  } else {
    log::warning("Uniform not found \"%s\"", name);
  }
}

GLuint Technique::gl_program() const
{
  return my_gl_program;
}

void Technique::locate_uniforms()
{
  GL_ERROR_GUARD;
  my_uniforms.clear();


  GLint count = 0;
  glGetProgramiv(my_gl_program, GL_ACTIVE_UNIFORMS, &count);


  for (int i = 0; i < count; ++i) {
    ShaderUniform u;

    if (get_shader_uniform_info(my_gl_program, i, u)) {
      log::info("Adding uniform %s", u.name.c_str());
      my_uniforms.push_back(u);
    }
  }
}

void set_uniform(const MetaField &meta_field, const void *data, GLint gl_location)
{
  switch (meta_field.type) {
// need implementation
//    case Type::F32: {
//    }
//    case Type::F32: {
//    }

    case Type::VEC2F: {
      const Vec2f &v = field_ref<Vec2f>(meta_field, data);
      glUniform2fv(gl_location, 1, &v[0]);
      break;
    }

    case Type::VEC3F: {
      const Vec3f &v = field_ref<Vec3f>(meta_field, data);
      glUniform3fv(gl_location, 1, &v[0]);
      break;
    }

    case Type::VEC4F: {
      const Vec4f &v = field_ref<Vec4f>(meta_field, data);
      glUniform4fv(gl_location, 1, &v[0]);
      break;
    }

    case Type::MAT4F: {
      const Mat4f &m = field_ref<Mat4f>(meta_field, data);
      glUniformMatrix4fv(gl_location, 1, false, &m[0][0]);
      break;
    }

    default:
      break;
  }
}

void Technique::pull(const MetaObject &properties)
{
  for (const ShaderUniform &u : my_uniforms) {
    const MetaField *meta_field = properties.meta_class.find_field(u.name.c_str());
    if (meta_field != nullptr) {
//      log::info("Pulling uniform %s", u.name.c_str());
      set_uniform(*meta_field, properties.data, u.gl_location);
    }
  }
}

Type Technique::get_type_from_gl_type(GLenum type)
{
  switch (type) {
    case GL_FLOAT:
      return Type::FLOAT;

    case GL_FLOAT_VEC2:
      return Type::VEC2F;

    case GL_FLOAT_VEC3:
      return Type::VEC3F;

    case GL_FLOAT_VEC4:
      return Type::VEC4F;

    case GL_FLOAT_MAT2:
      return Type::MAT2F;

    case GL_FLOAT_MAT3:
      return Type::MAT3F;

    case GL_FLOAT_MAT4:
      return Type::MAT4F;

    case GL_SAMPLER_2D:
      return Type::SAMPLER_2D;

    default:
      return Type::UNKNOWN;
  }
}

bool Technique::get_shader_uniform_info(GLuint gl_program, GLuint index,
                                        ShaderUniform &uniform)
{

  const int BUFFER_SIZE = 512;
  char name[BUFFER_SIZE];
  GLsizei length = 0;
  GLint size = 0;
  GLenum type = GL_INVALID_ENUM;
  glGetActiveUniform(gl_program, index, BUFFER_SIZE, &length, &size, &type, name);
  // skip uniforms with long names
  if (length == 0) {
    log::warning("Too long uniform name, skipping");
    return false;
  }

  GLint location = glGetUniformLocation(gl_program, name);

  if (location < 0) {
    log::warning("Can't find uniform location");
    return false;
  }

  Type uniform_type = get_type_from_gl_type(type);

  if (uniform_type == Type::UNKNOWN) {
    log::warning("Unknown uniform type \"%s\" (%i)", name, type);
    return false;
  }

  uniform.type = uniform_type;
  uniform.name = name;
  uniform.gl_location = location;
  return true;
}

const ShaderUniform *Technique::find_param(const char *name) const
{
  assert(name != nullptr);

  for (const ShaderUniform &uniform : my_uniforms) {
    if (!strcmp(name, uniform.name.c_str())) {
      return &uniform;
    }
  }

  return nullptr;
}

bool Technique::load_and_compile(const String &filename, Shader &shader)
{
  String src;
  return utils::load_file_into_string(filename, src) && shader.compile(src);
}

}
