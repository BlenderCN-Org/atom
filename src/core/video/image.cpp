#include "core/video/image.h"

#include <png.h>
#include <fstream>
#include "core/video/video_service.h"
#include "core/video/renderbuffer.h"
#include "core/video/framebuffer.h"
#include "core/constants.h"
#include "core/video/texture.h"
#include "core/log.h"

using std::ofstream;
using std::copy;
using std::swap;

namespace atom {

/**
 * Zatvor a uvolni vsetky zdroje pouzivane vo funkcii Image::create_from_file.
 *
 * @return vracia vzdy nullptr, aby bolo mozne pri chybe zavolat
 *         return exit_handler_create_from_file(...)
 */
void exit_handler_create_from_file(FILE *file, png_structp png_ptr, png_infop info_ptr)
{
  if (file != nullptr)
    fclose(file);

  if (png_ptr != nullptr)
    png_destroy_read_struct(&png_ptr, (info_ptr == nullptr ? nullptr : &info_ptr), nullptr);
}

uptr<Image> Image::create_from_file(
  const char *filename,
  bool bottom_left_first)
{
  assert(filename != nullptr);

  if (filename == nullptr)
    return nullptr;

  png_structp png_ptr = nullptr;
  png_infop info_ptr = nullptr;
  FILE *file = fopen(filename, "rb");

  // subor neexistuje
  if (file == nullptr) {
    log::warning("File doesn't exist \"%s\"", filename);
    return nullptr;
  }

  u8 signature[PNG_SIGNATURE_SIZE];

  // po uspesnom nacitani success == 1
  int success = fread(signature, PNG_SIGNATURE_SIZE, 1, file);

  if (success != 1) {
    log::warning("Can't read PNG signature from file \"%s\"", filename);
    exit_handler_create_from_file(file, png_ptr, info_ptr);
    return nullptr;
  }

  int png_status = png_sig_cmp((png_byte *) signature, 0, PNG_SIGNATURE_SIZE);

  if (png_status != 0) {
    log::warning("The \"%s\" is not a PNG image", filename);

    exit_handler_create_from_file(file, png_ptr, info_ptr);
    return nullptr;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  if (png_ptr == nullptr) {
    log::warning("Can't create \"png_struct\" structure - libpng error");

    exit_handler_create_from_file(file, png_ptr, info_ptr);
    return nullptr;
  }

  info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == nullptr) {
    log::warning("Can't create \"png_info\" structure - libpng error");

    exit_handler_create_from_file(file, png_ptr, info_ptr);
    return nullptr;
  }

  png_init_io(png_ptr, file);
  png_set_sig_bytes(png_ptr, PNG_SIGNATURE_SIZE);
  png_read_info(png_ptr, info_ptr);

  int width = png_get_image_width(png_ptr, info_ptr);
  int height = png_get_image_height(png_ptr, info_ptr);
  int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  int channels = png_get_channels(png_ptr, info_ptr);
  int color_type = png_get_color_type(png_ptr, info_ptr);
  PixelFormat format;

  if (channels == 3 && color_type == PNG_COLOR_TYPE_RGB) {
    log::debug(DEBUG_VIDEO, "RGB %s", filename);
    format = PixelFormat::RGB;
  } else if (channels == 4 && color_type == PNG_COLOR_TYPE_RGBA) {
    format = PixelFormat::RGBA;
    log::debug(DEBUG_VIDEO, "RGBA %s", filename);
  } else if (channels == 2 && color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
    log::debug(DEBUG_VIDEO, "RG %s", filename);
    format = PixelFormat::RG;
  } else if (channels == 1 && color_type == PNG_COLOR_TYPE_GRAY) {
    log::debug(DEBUG_VIDEO, "R %s", filename);
    format = PixelFormat::R;
  } else {
    log::warning("Unsupported PNG image, number of channels %i, color type %i", channels, color_type);

    exit_handler_create_from_file(file, png_ptr, info_ptr);
    return nullptr;
  }

  assert(row_bytes == width * pixel_size(format));

  uptr<Image> image(new Image(format, width, height));
  u8 *pixels = image->pixels();
  std::vector<u8 *> row_pointers(height, nullptr);

  // inicializuj row_pointers aby ukazoval do pixels
  if (bottom_left_first) {
    u8 *tmp = &pixels[height * row_bytes - row_bytes];

    for (int i = 0; i < height; ++i, tmp -= row_bytes)
      row_pointers[i] = tmp;
  } else {
    u8 *tmp = pixels;

    for (int i = 0; i < height; ++i, tmp += row_bytes)
      row_pointers[i] = tmp;
  }

  png_read_image(png_ptr, (png_bytepp) row_pointers.data());

  // uvolni a zatvor vsetky zdroje
  exit_handler_create_from_file(file, png_ptr, info_ptr);

  /// vrat ukazovatel na nacitany obrazok, @todo mozno vratit uptr
  return image;
}

Image::Image(
  PixelFormat format,
  int width,
  int height)
  : my_format(format)
  , my_width(width)
  , my_height(height)
  , my_is_owner(true)
{
  assert(format != PixelFormat::UNKNOWN);
  assert(width > 0);
  assert(height > 0);
  // alokuj pixle
  my_pixels.reset(new u8[width * height * pixel_size(format)]);
}

Image::Image(PixelFormat format, int width, int height, u8 *pixels)
  : my_format(format)
  , my_width(width)
  , my_height(height)
  , my_is_owner(false)
  , my_pixels(pixels)
{
  assert(format != PixelFormat::UNKNOWN);
  assert(width > 0);
  assert(height > 0);
  assert(pixels != nullptr);
}

int Image::pixel_size(PixelFormat format)
{
  switch (format) {
    case PixelFormat::RGB:
      return 3;

    case PixelFormat::R32I:
    case PixelFormat::RGBA:
      return 4;

    case PixelFormat::RGB32F:
      return 12;

    case PixelFormat::RGBA32UI:
    case PixelFormat::RGBA32F:
      return 16;

    case PixelFormat::D24S8:
      return 4;

    case PixelFormat::D16:
      return 2;

    case PixelFormat::RG:
      return 2;

    case PixelFormat::R:
      return 1;

    default:
      error("Unknown Image format %i", format);
  }

  return 0;
}

Image::~Image()
{
  // alokovana pamat je uvolnena automaticky ak som vlastnik, inak je za zrusenie data zodpovedny
  // ten kto vlastni pixlove data
  if (!my_is_owner)
    my_pixels.release();
}

int pixel_format_to_png_format(PixelFormat format)
{
  switch (format) {
    case PixelFormat::RGB:
      return PNG_COLOR_TYPE_RGB;

    case PixelFormat::RGBA:
      return PNG_COLOR_TYPE_RGBA;

    case PixelFormat::RG:
      return PNG_COLOR_TYPE_GRAY_ALPHA;

    case PixelFormat::R:
      return PNG_COLOR_TYPE_GRAY;

    default:
      error("There is no equivalent PNG format for format %i", format);
  }

  return PNG_COLOR_TYPE_RGB;
}

/**
 * Callback funkcia na zapis dat do PNG suboru. Je pouzita v Image::save_to_png.
 */
void png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  png_voidp ptr = png_get_io_ptr(png_ptr);

  if (ptr == nullptr)
    error("Can' write PNG data to nullptr file");

  ofstream *file = static_cast<ofstream *>(ptr);
  file->write(reinterpret_cast<const char *>(data), length);
}

bool Image::save_to_png(const String &filename)
{
  if (format() != PixelFormat::RGB && format() != PixelFormat::RGBA &&
      format() != PixelFormat::D16 && format() != PixelFormat::RG   &&
      format() != PixelFormat::R) {
    log::warning("Can't save image, only R/RG/RGB/RGBA/D16 formats are supported");
    return false;
  }

  ofstream file(filename, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);

  if (!file.is_open())
    error("Can't open file %s", filename.c_str());

  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  if (png_ptr == nullptr)
    error("Can't create PNG write struct");

  png_infop info_ptr = png_create_info_struct(png_ptr);

  if (info_ptr == nullptr) {
    png_destroy_write_struct(&png_ptr, (png_infopp) nullptr);
    error("Can't create PNG info write struct");
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    file.close();
    error("Error while writting Image to PNG file");
  }

  png_set_write_fn(png_ptr, &file, png_write_data, nullptr);
  png_set_compression_level(png_ptr, 9);    // best compression level
  png_set_compression_mem_level(png_ptr, 8);
//  png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
  png_set_compression_window_bits(png_ptr, 15);
  png_set_compression_method(png_ptr, 8);
  png_set_compression_buffer_size(png_ptr, 8192);

  // tu sa nastavuje bitova hlbka, teraz je na pevno 8, libpng zvladne aj 16
//  png_set_IHDR(png_ptr, info_ptr, width(), height(), 8, pixel_format_to_png_format(format()),
//      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
  int bits = 8;
  if (format() == PixelFormat::D16)
    bits = 16;

  png_set_IHDR(png_ptr, info_ptr, width(), height(), bits, pixel_format_to_png_format(format()),
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  uptr<png_bytep[]> rows(new png_bytep[height()]);int
  row_length = width() * pixel_size();

  for (int i = 0; i < height(); i++) {
    int offset = (height() - i - 1) * row_length;
    rows[i] = reinterpret_cast<png_bytep>(&(my_pixels[offset]));
  }

  png_set_rows(png_ptr, info_ptr, rows.get());
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);
  return true;
}

void copy_image(const Image &src, Image &dst)
{
  assert(src.width() == dst.width());
  assert(src.height() == dst.height());
  copy_image_rect(src, 0, 0, 0, 0, src.width(), src.height(), dst);
}

template<typename FromType, typename ToType>
void copy_image_rect_only_this_format(const Image &src, int src_x, int src_y, int dst_x, int dst_y,
  int width, int height, Image &dst, bool &copy_flag)
{
  // over formaty, ak sa zhoduju skopiruj data a nastav copy flag
  if (src.format() == FromType::pixel_format && dst.format() == ToType::pixel_format) {
    assert(copy_flag == false); // iba jedna funkcia moze robit kopiu
    const FromType *src_pixels = reinterpret_cast<const FromType *>(src.pixels());
    ToType *dst_pixels = reinterpret_cast<ToType *>(dst.pixels());
    // copy & ocnvert pixels
    convert_copy_rect(src.width(), src.height(), src_pixels, src_x, src_y, dst_x, dst_y,
      width, height, dst.width(), dst.height(), dst_pixels);
    // set copy flag
    copy_flag = true;
  }
}

void copy_image_rect(const Image &src, int src_x, int src_y, int dst_x, int dst_y,
  int width, int height, Image &dst)
{
  assert(src.width() > 0);
  assert(src.height() > 0);
  assert(dst.width() > 0);
  assert(dst.height() > 0);

  bool copy_flag = false;

  copy_image_rect_only_this_format<PixelRGB, PixelRGB>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);
  copy_image_rect_only_this_format<PixelRGBA, PixelRGB>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);

  copy_image_rect_only_this_format<PixelRGB, PixelRGBA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);
  copy_image_rect_only_this_format<PixelRGBA, PixelRGBA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);
  copy_image_rect_only_this_format<PixelD24S8, PixelRGBA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);

  copy_image_rect_only_this_format<PixelRGB, PixelBGRA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);
  copy_image_rect_only_this_format<PixelRGBA, PixelBGRA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);
  copy_image_rect_only_this_format<PixelBGRA, PixelBGRA>(src, src_x, src_y, dst_x, dst_y,
    width, height, dst, copy_flag);

  if (!copy_flag) {
    error("Unsupported source image format %i", src.format());
  }
}

}
