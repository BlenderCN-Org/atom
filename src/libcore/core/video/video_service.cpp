#include "video_service.h"

#include <cstring>

#include "texture.h"
#include "uniforms.h"
#include "framebuffer.h"
#include "renderbuffer.h"
#include "video_buffer.h"
#include "texture_sampler.h"
#include "technique.h"
#include "../utils/gl_utils.h"

namespace atom {
namespace {

/**
 * Zisti pozadovane limity OpenGL, napr. maximalny pocet textur, velkost textur, ...
 *
 * @return true ak OpenGL splna pozadovane limity, inak false (momentalne
 *         sa vypise chybova hlaska a program skonci pred tym nez vrati false)
 */
bool check_gl_limits()
{
  // print OpenGL info
  log::info("OpenGL %s %s", glGetString(GL_VERSION), glGetString(GL_RENDERER));

  GLint gl_max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
  log::debug(DEBUG_VIDEO, "Max texture size %i", gl_max_texture_size);

  if (gl_max_texture_size < 4096) {
    error("Big textures aren't supported");
    return false;
  }

  GLint gl_max_vertex_attribs;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &gl_max_vertex_attribs);
  log::debug(DEBUG_VIDEO, "Max vertex attributes %i", gl_max_vertex_attribs);
  // verify that graphic card support enought vertex attributes
  if (gl_max_vertex_attribs < REQ_VERTEX_ATTRIBUTES) {
    error("Not enough vertex attributes");
    return false;
  }

  //QQQ
//  if (!GLEW_ARB_shading_language_420pack) {
//    error("ARB_shading_language_420pack not present");
//    return false;
//  }

  GLint gl_max_vertex_uniform_components = 0;
  GLint gl_max_geometry_uniform_components = 0;
  GLint gl_max_fragment_uniform_components = 0;

  glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &gl_max_vertex_uniform_components);
  glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &gl_max_geometry_uniform_components);
  glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &gl_max_fragment_uniform_components);

  log::debug(DEBUG_VIDEO, "Max uniform components vertex %i, geometry %i, fragment %i",
    gl_max_vertex_uniform_components, gl_max_geometry_uniform_components,
    gl_max_fragment_uniform_components);

  GLint gl_max_draw_buffers;
  glGetIntegerv(GL_MAX_DRAW_BUFFERS, &gl_max_draw_buffers);
  log::debug(DEBUG_VIDEO, "Max draw buffers %i", gl_max_draw_buffers);

  GLint gl_max_elements_indices;
  glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &gl_max_elements_indices);
  log::debug(DEBUG_VIDEO, "Max elements indices %i", gl_max_elements_indices);

  GLint gl_max_texture_buffer_size;
  glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &gl_max_texture_buffer_size);
  log::debug(DEBUG_VIDEO, "Max texture buffer size %i", gl_max_texture_buffer_size);

  return true;
}

} // anonymous namespace

GLfloat SQUARE_VERTICES[] = {
    -0.5, 0.5, 0.0,
    0.5, 0.5, 0.0,
    0.5, -0.5, 0.0,
    0.5, -0.5, 0.0,
    -0.5, -0.5, 0.0,
    -0.5, 0.5, 0.0
};

VideoService::VideoService()
  : my_uniforms(new Uniforms())
{
  memset(&my_state, 0, sizeof(State));
  // initialize with nullptr
  for (unsigned i = 0; i < TEXTURE_UNIT_COUNT; ++i) {
    my_textures[i] = nullptr;
    my_samplers[i] = nullptr;
  }

  GL_ERROR_GUARD;

  if (!check_gl_limits()) {
    error("Missing OpenGL extensions");
    return;
  }

  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
}

VideoService::~VideoService()
{
}

void VideoService::draw(const DrawCommand &command)
{
  if (command.program == nullptr) {
    log::warning("DrawCommand without program");
    return;
  }

  for (u32 i = 0; i < MAX_ATTRIBUTES; ++i) {
    if (command.attributes[i] != nullptr) {
      bind_attribute(i, *command.attributes[i], command.types[i]);
    } else {
      unbind_attribute(i);
    }
  }

  bind_program(*command.program);
  command.program->pull(meta_object(*my_uniforms));

  if (command.draw == DrawType::TRIANGLES) {
    draw_index_array(GL_TRIANGLES, *command.indices, command.indices->size() / sizeof(u32));
  } else if (command.draw == DrawType::LINES) {
    draw_index_array(GL_LINES, *command.indices, command.indices->size() / sizeof(u32));
  } else {
    log::warning("DrawCommand is missing primitive type");
  }
}

void VideoService::bind_program(Technique &program)
{
  GL_ERROR_GUARD;

  if (my_state.program == &program) {
    return;
  }

  my_state.program = &program;
  glUseProgram(program.gl_program());
}

void VideoService::unbind_program()
{
  GL_ERROR_GUARD;
  my_state.program = nullptr;
  glUseProgram(0);
}

void VideoService::bind_texture(u32 index, const Texture &texture)
{
  assert(index < TEXTURE_UNIT_COUNT);
  GL_ERROR_GUARD;
  // texture_unit zacina hodnotou 0, preto ho treba skonvertovat na GL_TEXTURE0..i
  glActiveTexture(GL_TEXTURE0 + index);

  TextureType type = texture.type();
  if (type == TextureType::BUFFER)
    glBindTexture(GL_TEXTURE_BUFFER, texture.gl_texture());
  else if (type == TextureType::TEXTURE_2D)
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture());
  else if (type == TextureType::RECTANGLE)
    glBindTexture(GL_TEXTURE_RECTANGLE, texture.gl_texture());
  else
    error("This texture type is not supported %i (%s)", type, __FUNCTION__);

  my_textures[index] = &texture;
}

void VideoService::unbind_texture(u32 index)
{
  assert(index < TEXTURE_UNIT_COUNT);
  GL_ERROR_GUARD;
  const Texture *texture = my_textures[index];

  if (texture != nullptr) {
    // texture_unit zacina hodnotou 0, preto ho treba skonvertovat na GL_TEXTURE0..i
    glActiveTexture(GL_TEXTURE0 + index);
    TextureType type = texture->type();
    if (type == TextureType::BUFFER) {
      glBindTexture(GL_TEXTURE_BUFFER, 0);
    } else if (type == TextureType::TEXTURE_2D) {
      glBindTexture(GL_TEXTURE_2D, 0);
    } else if (type == TextureType::RECTANGLE) {
      glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    } else {
      log::warning("This texture type is not supported %i (%s)", type, ATOM_FUNC_NAME);
    }
    my_textures[index] = nullptr;
  }
}

void VideoService::bind_sampler(u32 index, const TextureSampler &sampler)
{
  assert(my_state.program != nullptr && "Program must be bound");
  assert(index < TEXTURE_UNIT_COUNT);

  my_samplers[index] = &sampler;
  glBindSampler(index, sampler.gl_sampler());
}

void VideoService::unbind_sampler(u32 index)
{
  assert(index < TEXTURE_UNIT_COUNT);
  GL_CHECK_ERROR;
  if (my_samplers[index] == nullptr) {
    return;
  }

  glBindSampler(index, 0);
}

void VideoService::bind_attribute(u32 index, const VideoBuffer &buffer, Type type)
{
  glEnableVertexAttribArray(index);
  bind_array_buffer(buffer);

  switch (type) {
    case Type::VEC2F:
      glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, 0, 0);
      break;

    case Type::VEC3F:
      glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, 0, 0);
      break;

    case Type::VEC4F:
      glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, 0);
      break;

      //QQQ bacha na int, int vyzaduje volanie glVertexAttribIPointer!!!
    case Type::U32:
//      glVertexAttribIPointer(index, 1, GL_UNSIGNED_INT, GL_FALSE, 0, 0);
      glVertexAttribIPointer(index, 1, GL_UNSIGNED_INT, 0, 0);
      break;

    case Type::VEC4U8:
      glVertexAttribIPointer(index, 4, GL_UNSIGNED_BYTE, 0, 0);
      break;

    default:
      log::error("Unsupported attribute type %i", static_cast<int>(type));
      break;
  }

  unbind_array_buffer();
}

void VideoService::unbind_attribute(u32 index)
{
  glDisableVertexAttribArray(index);
}

void VideoService::bind_array_buffer(const VideoBuffer &buffer)
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer.gl_buffer());
}

void VideoService::unbind_array_buffer()
{
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//void VideoService::bind_index_buffer(IndexBuffer &buffer)
//{
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.gl_buffer());
//}

//void VideoService::unbind_index_buffer()
//{
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}

//void VideoService::bind_pixel_pack_buffer(PixelBuffer &buffer)
//{
//  glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, buffer.gl_buffer());
//  active_pixel_pack_buffer = &buffer;
//}

//void VideoService::unbind_pixel_pack_buffer()
//{
//  glBindBuffer(GL_PIXEL_PACK_BUFFER_ARB, 0);
//  active_pixel_pack_buffer = nullptr;
//}

//void VideoService::bind_pixel_unpack_buffer(PixelBuffer &buffer)
//{
//  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer.gl_buffer());
//  active_pixel_unpack_buffer = &buffer;
//}

//void VideoService::unbind_pixel_unpack_buffer()
//{
//  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
//  active_pixel_unpack_buffer = nullptr;
//}

void VideoService::bind_renderbuffer(Renderbuffer &renderbuffer)
{
  GL_ERROR_GUARD;
  glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer.gl_renderbuffer());
  my_state.renderbuffer = &renderbuffer;
}

void VideoService::unbind_renderbuffer()
{
  GL_ERROR_GUARD;
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  my_state.renderbuffer = nullptr;
}

void VideoService::bind_read_framebuffer(Framebuffer &framebuffer, bool verify)
{
  GL_ERROR_GUARD;
  if (&framebuffer != my_state.read_framebuffer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer.get_gl_framebuffer());
    my_state.read_framebuffer = &framebuffer;
  }

  if (verify && !is_read_framebuffer_complete()) {
    error("Read framebuffer is not complete");
    return;
  }
}

void VideoService::unbind_read_framebuffer()
{
  GL_ERROR_GUARD;
  if (my_state.read_framebuffer != nullptr) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    my_state.read_framebuffer = nullptr;
  }
}

void VideoService::set_read_buffer(u32 index)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0 + index);
}

void VideoService::bind_write_framebuffer(Framebuffer &framebuffer, bool verify)
{
  GL_ERROR_GUARD;
  if (&framebuffer != my_state.write_framebuffer)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.get_gl_framebuffer());
  // vytvor z bitovej masky color attachment-ov pole draw buffers
  std::vector<GLenum> draw_buffers;
  unsigned color_attachments = framebuffer.color_attachments();
  for (u32 i = 0; i < sizeof(u32) * 8; ++i) {
    if (color_attachments & (1 << i))
      draw_buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
  }
  // zadaj do ktorych attachmentov sa bude kreslit
  glDrawBuffers(draw_buffers.size(), &draw_buffers[0]);
  // over uplnost (completness) framebufferu, ci je pripraveny na kreslenie
  if (verify && !is_draw_framebuffer_complete()) {
    error("Write framebuffer is not complete");
    return;
  }
  my_state.write_framebuffer = &framebuffer;
}

void VideoService::unbind_write_framebuffer()
{
  if (my_state.write_framebuffer != nullptr) {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    my_state.write_framebuffer = nullptr;
  }
}

void VideoService::set_blending(BlendOperation src, BlendOperation dst)
{
  if (src == BlendOperation::NO_BLENDING) {
    glDisable(GL_BLEND);
  } else {
    glEnable(GL_BLEND);
    glBlendFunc(static_cast<GLenum>(src), static_cast<GLenum>(dst));
  }
}

void VideoService::disable_blending()
{
  glDisable(GL_BLEND);
}

void VideoService::enable_depth_test()
{
  glEnable(GL_DEPTH_TEST);
}

void VideoService::disable_depth_test()
{
  glDisable(GL_DEPTH_TEST);
}

void set_viewport(int x, int y, int width, int height)
{
  glViewport(x, y, width, height);
}

void VideoService::draw_arrays(GLenum mode, GLint first, GLsizei count)
{
  /// @todo assert that at least one array is active
  glDrawArrays(mode, first, count);
}

void VideoService::draw_index_array(GLenum gl_mode, const VideoBuffer &buffer, u32 count)
{
  GL_ERROR_GUARD;

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.gl_buffer());
  glDrawElements(gl_mode, count, GL_UNSIGNED_INT, nullptr);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Uniforms& VideoService::get_uniforms()
{
  return *my_uniforms;
}

void VideoService::set_draw_face(DrawFace face)
{
  switch (face) {
    case DrawFace::FRONT:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_BACK);
      break;

    case DrawFace::BACK:
      glEnable(GL_CULL_FACE);
      glCullFace(GL_FRONT);
      break;

    case DrawFace::BOTH:
      glDisable(GL_CULL_FACE);
      break;
  }
}

AttributeBinder::AttributeBinder(VideoService &vs, int index, const VideoBuffer &buffer, Type type)
  : my_vs(vs)
  , my_index(index)
{
  my_vs.bind_attribute(my_index, buffer, type);
}

AttributeBinder::~AttributeBinder()
{
  my_vs.unbind_attribute(my_index);
}

}
