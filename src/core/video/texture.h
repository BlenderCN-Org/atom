#pragma once

#include "core/corefwd.h"
#include "core/video/image.h"
#include "core/video/video_buffer.h"

namespace atom {

/**
 * Druhy textur.
 */
enum class TextureType {
  UNKNOWN,
  TEXTURE_2D,
  BUFFER,
  RECTANGLE
};

class Texture : private NonCopyable {
public:
  // public methods
  Texture(VideoService &vs);

  /**
   * Inicializuj texturu ako RGBA obrazok (2d).
   */
  void init_as_texture_2d(PixelFormat format, int width, int height);

  void init_from_image(const Image &image);

//  static uptr<Texture> create_depth_texture(
//    VideoService &vs,
//    int width,
//    int height);

  /**
   * Create empty 2d texture.
   *
   * Vytvorena textura je pripojena vo VideoProcesor-e.
   * @todo prepisat na konstruktor ???
   */
//  static uptr<Texture> create_texture_2d(
//    VideoService &vs,
//    PixelFormat format,
//    int width,
//    int height,
//    GLint gl_min_filter = DEFAULT_GL_MIN_FILTER,
//    GLint gl_mag_filter = DEFAULT_GL_MAG_FILTER,
//    GLint gl_wrap_s_mode = DEFAULT_GL_WRAP_S,
//    GLint gl_wrap_t_mode = DEFAULT_GL_WRAP_T);

  /**
   * @todo prepisat na konstruktor ???
   */
  //QQQ
//  static Texture* create_texture_rectangle(
//    VideoService &vs,
//    PixelFormat format,
//    int width,
//    int height,
//    GLint gl_min_filter = DEFAULT_GL_MIN_FILTER,
//    GLint gl_mag_filter = DEFAULT_GL_MAG_FILTER,
//    GLint gl_wrap_s_mode = DEFAULT_GL_WRAP_S,
//    GLint gl_wrap_t_mode = DEFAULT_GL_WRAP_T);

  /**
   * Create empty texture with associated buffer object.
   *
   * This will bind the new texture and the texture buffer into the VideoService.
   * @todo prepisat na konstruktor ???
   */
//  QQQ
//  static Texture* create_texture_buffer(
//    VideoService &vs,
//    PixelFormat format,
//    int width,
//    int height);

  /**
   * Destruktor, uvolni texturu z pamate OpenGL.
   */
  ~Texture();

  /**
   * Nahraj data do textury.
   */
  void set_data(PixelFormat format, int width, int height, const void *data);
  /* @todo
  void set_data_rgba(int width, int height, const PixelRGBA *data);
  void set_data_rgb(int width, int height, const PixelRGB *data);
  void set_data_rgba32ui(int width, int height, const PixelRGBA32UI *data);
  */

  /**
   * Vrat type textury, bud TEXTURE_2D, BUFFER.
   */
  TextureType type() const
  { return my_type; }

  /**
   * Vrat pixel format textury. Nie vsetky PixelFormat hodnoty su podporovane.
   */
  PixelFormat format() const
  { return my_format; }

  /**
   * Sirka v pixeloch (sirka pouziteho obrazku).
   */
  int width() const
  { return my_width; }

  /**
   * Vyska v pixeloch (vyska pouziteho obrakzu).
   */
  int height() const
  { return my_height; }

  /**
   * Pomer sirky k vyske.
   */
  float aspect_ratio() const
  { return static_cast<float>(my_width) / my_height; }

  size_t size() const
  { return pixel_data_size(format(), width(), height()); }

//  TextureBuffer* texture_buffer()
//  { return my_texture_buffer.get(); }

  /**
   * Interny OpenGL identifikator textury, pouzite v OpenGL volaniach,
   * napr. v glBindTexture...
   */
  GLuint gl_texture() const
  { return my_gl_texture; }

  /**
   * Hodnota format pre funkciu glTexImage2D
   */
  GLenum get_gl_data_format() const
  { return pixel_format_to_gl_data_format(my_format); };

  GLenum get_gl_data_type() const
  { return pixel_format_to_gl_data_type(my_format); };

  static GLint pixel_format_to_gl_format(
    PixelFormat format);

  static GLint pixel_format_to_gl_data_format(
    PixelFormat format);

  static GLint pixel_format_to_gl_data_type(
    PixelFormat format);

private: // private methods
  /**
   * Konstruktor, vytvori OpenGL texturu, ale nenahra do nej ziadne data.
   * Miesto na data v pamati sa alokuje az pri prvom nahrati dat.
   */
//  Texture(TextureType type, PixelFormat format, int width, int height);

  static std::size_t pixel_data_size(
    PixelFormat format,
    int width,
    int height);

  static GLenum texture_type_to_gl_type(TextureType type);

  GLenum gl_texture_type() const
  { return texture_type_to_gl_type(type()); }

private:// private constants
  static const GLint DEFAULT_GL_MIN_FILTER = GL_LINEAR;
  static const GLint DEFAULT_GL_MAG_FILTER = GL_LINEAR;
  static const GLint DEFAULT_GL_WRAP_S = GL_CLAMP_TO_EDGE;
  static const GLint DEFAULT_GL_WRAP_T = GL_CLAMP_TO_EDGE;
//  static const GLint DEFAULT_GL_WRAP_S = GL_REPEAT;
//  static const GLint DEFAULT_GL_WRAP_T = GL_REPEAT;

private:// private members
  VideoService       &my_vs;
  TextureType         my_type;           ///< typ 2d, buffer, ...
  PixelFormat         my_format;         ///< format pixlov
  int                 my_width;          ///< sirka textury
  int                 my_height;         ///< vyska textury
  GLuint              my_gl_texture;     ///< OpenGL indentifikator textury
//  uptr<TextureBuffer> my_texture_buffer; ///< texture buffer
};

}
