#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include "../corefwd.h"
#include "../utils/utils.h"

namespace atom {

class Uniforms;

enum class FramebufferTarget {
  INVALID = -1,
  READ = GL_READ_FRAMEBUFFER,
  DRAW = GL_DRAW_FRAMEBUFFER
};

inline GLenum framebuffer_target_to_gl(FramebufferTarget target)
{
  assert(target != FramebufferTarget::INVALID);
  return static_cast<GLenum>(target);
}

const u32 TEXTURE_UNIT_COUNT = 8; ///QQQ ziskat z opengl a este pridat sampler_count
const u32 TEXTURE_SAMPLER_COUNT = 8;

enum class DrawFace {
  FRONT,
  BACK,
  BOTH
};

enum class BlendOperation : GLenum {
  NO_BLENDING = 0,
  SRC_ALPHA = GL_SRC_ALPHA,
  ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
};

class VideoService : private NonCopyable {
public:
  VideoService();

  ~VideoService();

  void bind_program(Technique &program);

  void unbind_program();

  /**
   * Nastav alebo zrus 2D texturu.
   *
   * @param texture nova nastavena textura, alebo hodnota nullptr, ktora odstrani texturu z texturovacej jednotky
   * @param texture_unit texturovaci slot pre texturu
   * @param gl_uniform_sampler hodnota > 0 je ekvivalent volaniu glUniform1v(gl_uniform_sampler, ...)
   */
  void bind_texture(u32 index, const Texture &texture);

  void unbind_texture(u32 index);

  void bind_sampler(u32 index, const TextureSampler &sampler);

  void unbind_sampler(u32 index);

  void bind_attribute(u32 index, const VideoBuffer &buffer, Type type);

  void unbind_vertex_attribute(u32 index);

//  void bind_texture_buffer(TextureBuffer &texture_buffer);

//  void unbind_texture_buffer();

  void bind_array_buffer(const VideoBuffer &buffer);

  void unbind_array_buffer();

//  void bind_index_buffer(IndexBuffer &buffer);

//  void unbind_index_buffer();

//  void bind_pixel_pack_buffer(PixelBuffer &buffer);

//  void unbind_pixel_pack_buffer();

//  void bind_pixel_unpack_buffer(PixelBuffer &buffer);

//  void unbind_pixel_unpack_buffer();

  void bind_renderbuffer(Renderbuffer &renderbuffer);

  void unbind_renderbuffer();

  void bind_read_framebuffer(Framebuffer &framebuffer, bool verify = true);

  void unbind_read_framebuffer();

  void set_read_buffer(u32 index);

  /**
   * Pripoj vystupny framebuffer na kreslenie.
   * Parameter verify urcuje ci sa ma otestovat uplnost/completness
   * framebufferu. Ak je verify nastavene na true a framebuffer je neuplny
   * (status != GL_FRAMEBUFFER_COMPLETE) tak program skonci s chybovou hlaskou.
   */
  void bind_write_framebuffer(Framebuffer &framebuffer, bool verify = true);

  void unbind_write_framebuffer();

  void set_blending(BlendOperation src, BlendOperation dst);

  void disable_blending();

  void enable_depth_test();

  void disable_depth_test();

  void set_viewport(int x, int y, int width, int height);

  void draw_arrays(GLenum mode, GLint first, GLsizei count);

  void draw_index_array(GLenum gl_mode, const VideoBuffer &buffer, u32 count);

  Uniforms& get_uniforms();

  void set_draw_face(DrawFace face);

  struct State {
    Technique   *program;
    Texture        *textures[TEXTURE_UNIT_COUNT];
    TextureSampler *samples[TEXTURE_SAMPLER_COUNT];
    Renderbuffer   *renderbuffer;
    Framebuffer    *read_framebuffer;
    Framebuffer    *write_framebuffer;
  };

private:
  State          my_state;
  const Texture      *my_textures[TEXTURE_UNIT_COUNT];
  const TextureSampler *my_samplers[TEXTURE_UNIT_COUNT];
  uptr<Uniforms> my_uniforms;
};

template<>
inline Type type_of<DrawFace>()
{
  return Type::DRAW_FACE;
}

}
