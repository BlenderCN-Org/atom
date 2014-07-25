#include "texture.h"

#include "video_service.h"
#include "image.h"

using std::swap;

namespace atom {

Texture::Texture(VideoService &vs)
  : my_vs(vs)
  , my_type(TextureType::UNKNOWN)
  , my_format(PixelFormat::UNKNOWN)
  , my_width(-1)
  , my_height(-1)
{
  glGenTextures(1, &my_gl_texture);
}

void Texture::init_as_texture_2d(PixelFormat format, int width, int height)
{
  assert(width > 0);
  assert(height > 0);

  GL_ERROR_GUARD;

  my_type = TextureType::TEXTURE_2D;
  my_format = format;
  my_width  = width;
  my_height = height;

  my_vs.bind_texture(0, *this);
  GL_CHECK_ERROR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, DEFAULT_GL_MIN_FILTER);
  GL_CHECK_ERROR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, DEFAULT_GL_MAG_FILTER);
  GL_CHECK_ERROR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, DEFAULT_GL_WRAP_S);
  GL_CHECK_ERROR;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, DEFAULT_GL_WRAP_T);
  GL_CHECK_ERROR;
  // allocate texture memory, !!! vsetky parametre su dolezite, aj ked nenastavujeme data
  glTexImage2D(GL_TEXTURE_2D, 0, pixel_format_to_gl_format(format), my_width, my_height,
    0, pixel_format_to_gl_data_format(format), pixel_format_to_gl_data_type(format), nullptr);
  GL_CHECK_ERROR;

}

void Texture::init_from_image(const Image &image)
{
  init_as_texture_2d(image.format(), image.width(), image.height());
  set_data(image.format(), my_width, my_height, image.pixels());
}

Texture::~Texture()
{
  // glDeleteTextures ignoruje 0, takze nieje potrebne testovat tuto variantu
  glDeleteTextures(1, &my_gl_texture);
}

void Texture::set_data(PixelFormat format, int width, int height, const void *data)
{
  if (my_type != TextureType::TEXTURE_2D) {
    log::error("Texture::set_data called on non 2D texture");
    return;
  }

  my_vs.bind_texture(0, *this);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, my_width, my_height, 0,
    pixel_format_to_gl_data_format(format), pixel_format_to_gl_data_type(format), data);
}

GLint Texture::pixel_format_to_gl_format(PixelFormat format)
{
  switch (format) {
    case PixelFormat::R:
      return GL_RED;

    case PixelFormat::RG:
      return GL_RG;

    case PixelFormat::RG16F:
      return GL_RG16F;

    case PixelFormat::RGB:
      return GL_RGB;

    case PixelFormat::RGBA:
      return GL_RGBA;

    case PixelFormat::RGBA32F:
      return GL_RGBA32F;

    case PixelFormat::RGB32F:
      return GL_RGB32F;

    case PixelFormat::RGBA32UI:
      return GL_RGBA32UI;

    case PixelFormat::R32I:
      return GL_R32I;

    case PixelFormat::RGBA_DXT5:
      return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    case PixelFormat::D16:
      return GL_DEPTH_COMPONENT16;  // mozno by mohlo byt aj GL_DEPTH_COMPONENT

    case PixelFormat::D32:
      return GL_DEPTH_COMPONENT32;  // mozno by mohlo byt aj GL_DEPTH_COMPONENT

    case PixelFormat::D24S8:   // experimentalne, zatial neotestovane, treba odskusat
      return GL_DEPTH24_STENCIL8;

    case PixelFormat::D32F:   // experimentalne, zatial neotestovane, treba odskusat
      return GL_DEPTH_COMPONENT32F;

    default:
      log::warning("This pixel format is not supported %i", format);
      return -1;
  }
}

GLint Texture::pixel_format_to_gl_data_format(PixelFormat format)
{
  switch (format) {
    case PixelFormat::R:
      return GL_RED;

    case PixelFormat::RG:
    case PixelFormat::RG16F:
      return GL_RG;

    case PixelFormat::RGB:
    case PixelFormat::RGB32F:
      return GL_RGB;

    case PixelFormat::RGBA:
    case PixelFormat::RGBA32F:
      return GL_RGBA;

    case PixelFormat::RGBA32UI:
      return GL_RGBA_INTEGER;

    case PixelFormat::R32I:
      return GL_RED_INTEGER;

    case PixelFormat::D16:
    case PixelFormat::D32:
    case PixelFormat::D32F: // experimentalne, neotestovane, treba odskusat
      return GL_DEPTH_COMPONENT;

    case PixelFormat::D24S8: // experimentalne, neotestovane, treba odskusat
      return GL_DEPTH_STENCIL;

    default:
      log::warning("This pixel format is not supported %i", format);
      return -1;
  }
}

GLint Texture::pixel_format_to_gl_data_type(PixelFormat format)
{
  switch (format) {
    case PixelFormat::R:
    case PixelFormat::RG:
    case PixelFormat::RGB:
    case PixelFormat::RGBA:
      return GL_UNSIGNED_BYTE;

    case PixelFormat::RG16F:
      return GL_HALF_FLOAT;

    case PixelFormat::RGB32F:
    case PixelFormat::RGBA32F:
      return GL_FLOAT;

    case PixelFormat::RGBA32UI:
      return GL_UNSIGNED_INT;

    case PixelFormat::R32I:
      return GL_INT;

    case PixelFormat::D16:
      return GL_UNSIGNED_SHORT;

    case PixelFormat::D32:
      return GL_UNSIGNED_INT;

    case PixelFormat::D24S8:
      return GL_UNSIGNED_INT_24_8;

    case PixelFormat::D32F:
      return GL_FLOAT;

    default:
      log::warning("This pixel format is not supported %i", format);
      return -1;
  }
}

std::size_t Texture::pixel_data_size(PixelFormat format, int width, int height)
{
  assert(width > 0);
  assert(height > 0);

  return width * height * Image::pixel_size(format);
}

GLenum Texture::texture_type_to_gl_type(TextureType type)
{
  switch (type) {
    case TextureType::TEXTURE_2D:
      return GL_TEXTURE_2D;

    case TextureType::BUFFER:
      return GL_TEXTURE_BUFFER;

    case TextureType::RECTANGLE:
      return GL_TEXTURE_RECTANGLE;

    case TextureType::UNKNOWN:
      error("Unknown type doesn't have OpenGL equivalent");
  }
  error("Texture type not supported");
  return GL_INVALID_ENUM;
}

}
