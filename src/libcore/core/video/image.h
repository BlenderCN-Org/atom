#pragma once

#include <cassert>
#include "../foundation.h"
#include "pixel.h"

namespace atom {

/**
 * Obecna konverzna funkcia pixel formatov.
 * Na prevod z jedneho formatu do druheho sa pouziva pretazeny operator=
 * Sirka a vyska zdroja a ciela musia byt zhodne.
 * Parametre sablony nieje nutne zadavat, odvodia sa automaticky z parametrov
 * funkcie. Kvoli zvyseniu bezpecnosti je ich mozne zadat manualne, aby sme mali
 * zarucene ze sa zhoduju z predanymi typmi src a dst.
 *
 * @param FromType datovy typ pixlu z ktoreho sa bude vykona konverzia, napr. PixelRGB...
 * @param ToType datovy typ cieloveho pixlu, napr. PixelRGBA, ...
 * @param width sirka obrakzu
 * @param height vyska obrakzu
 * @param src ukazovatel na pole zdrojovych pixlov
 * @param dst ukazovatel na miesto kam sa ulozi vysledok
 */
template<class FromType, class ToType>
inline void convert_copy(
  int width,
  int height,
  const FromType *src,
  ToType *dst)
{
  std::copy(&src[0], &src[width * height], dst);
}

/**
 * Skopiruj obldznikovy vyrez (urob konverziu pixlov, ak je nutna).
 * Vsetky rozmery su v pixloch (nie v bytoch).
 */
template<class FromType, class ToType>
inline void convert_copy_rect(
  int src_width,
  int src_height,
  const FromType *src_pixels,
  int src_x,
  int src_y,
  int dst_x,
  int dst_y,
  int width,
  int height,
  int dst_width,
  int dst_height,
  ToType *dst_pixels)
{
  assert(src_pixels != nullptr);
  assert(dst_pixels != nullptr);
  assert(width <= (src_width - src_x));
  assert(width <= (dst_width - dst_x));
  assert(height <= (src_height - src_y));
  assert(height <= (dst_height - dst_y));

  const FromType *src = src_pixels + src_y * src_width;
  ToType *dst = dst_pixels + dst_y * dst_width;
  for (int i = 0; i < height; ++i) {
    std::copy(src + src_x, src + src_x + width, dst + dst_x);
    src += src_width;
    dst += dst_width;
  }
}

/**
 * Skopiruj obldznikovy vyrez horizontalne obrateny (urob konverziu pixlov, ak je nutna).
 * Vsetky rozmery su v pixloch (nie v bytoch).
 */
template<class FromType, class ToType>
inline void convert_copy_flip_rect(
  int src_width,
  int src_height,
  const FromType *src_pixels,
  int src_x,
  int src_y,
  int dst_x,
  int dst_y,
  int width,
  int height,
  int dst_width,
  int dst_height,
  ToType *dst_pixels)
{
  assert(src_pixels != nullptr);
  assert(dst_pixels != nullptr);
  assert(width <= (src_width - src_x));
  assert(width <= (dst_width - dst_x));
  assert(height <= (src_height - src_y));
  assert(height <= (dst_height - dst_y));

  const FromType *src = src_pixels + (src_y + height - 1) * src_width;
  ToType *dst = dst_pixels + dst_y * dst_width;
  for (int i = 0; i < height; ++i) {
    std::copy(src + src_x, src + src_x + width, dst + dst_x);
    src -= src_width;
    dst += dst_width;
  }
}

/**
 * Pamatovy obrazok, nacitany z PNG suboru
 */
class Image : private NonCopyable {
public:   // public methods
  /**
   * Nacitaj PNG obrazok z disku.
   *
   * @param filename cesta k obrazku, musi obsahovat priponu (.png)
   * @return
   */
  static uptr<Image> create_from_file(
    const char *filename,
    bool bottom_left_first = true);

  /**
   * Vytvor novy obrazok zo zadanym formatom a rozmermi.
   *
   * Vytvoreny obrazok ma alokovau pamat pre pixle. Ich hodnota je vsak nieje definovana.
   *
   * @param format typ obrazku
   * @param width sirka obrazku
   * @param height vyska obrazku
   */
  Image(
    PixelFormat format,
    int width,
    int height);

  /**
   * Vytvor obrazok s existujucich pixlov.
   *
   * Format, vyska a sirka urcuju ako sa bude pristupovat k datam. Za zrusenie dat nie je zodpovedny
   * Image, ale ten kto vytvara Image.
   *
   * @param format typ obrazku
   * @param width sirka obrazku
   * @param height vyska obrazku
   * @param pixels pixelove data (nezabudnut zrusit)
   */
  Image(
    PixelFormat format,
    int width,
    int height,
    u8 *pixels);

  /// Destrukor, uvolni pouzitu pamat
  ~Image();

  /// Sirka obrazku
  int width() const
  { return my_width; }

  /// Vyska obrazku
  int height() const
  { return my_height; }

  PixelFormat format() const
  { return my_format; }

  /// Velkost dat obrazku v bytoch
  int get_size() const
  { return my_width * my_height * pixel_size(my_format); }

  /// Dlzka riadku v bytoch
  int get_row_length() const
  { return my_width * pixel_size(); }

  /// Pixely obrazku
  const u8 * pixels() const
  { return my_pixels.get(); }

  u8 * pixels()
  { return my_pixels.get(); }

  int pixel_size() const
  { return pixel_size(format()); }

  bool save_to_png(const String &filename);

  /**
   * Vypocitaj velkost pixelu v danom formate.
   *
   * @return velkost pixelu v bytoch, pri neznamom formate vrati 0
   *         (este predtym ale vypise chybu a skonci)
   */
  static int pixel_size(PixelFormat format);

private:  // private methods

  /**
   * Z OpenGL formatu textury urci format obrazku.
   * Vhodne pri konverzii textury na obrazok.
   *
   * @return prislusny format, pri chybe vrati UNKNOWN
   *         (este predtym vypise chybu a skonci)
   */
//  static PixelFormat gl_format_to_pixel_format(GLenum gl_format);

private:  // private members
  PixelFormat my_format;
  int         my_width;
  int         my_height;
  bool        my_is_owner;  ///< patria mi pixely (data v my_pixels)
  uptr<u8[]>  my_pixels;
};

/**
 * Skopiruj data jedneho obrazku do druheho. Ak maju obrazky rozny format urob aj konverziu.
 *
 * @param src zdrojovy obrazok
 * @param dst cielovy obrazok
 */
void copy_image(
  const Image &src,
  Image &dst);

/**
 * Skopiruj cast zdrojoveho obrazku. Ak maju obrazky rozny format, urob aj konverziu pixlov.
 *
 * @param src zdrojovy obrazok
 * @param src_x pociatok zdrojoveho obdlznika
 * @param src_y pociatok zdrojoveho obdlznika
 * @param dst_x pociatok cieloveho obdlznika
 * @param dst_y pociatok cieloveho obdlznika
 * @param width sitrka kopirovanej oblasti v pixloch
 * @param height vyska kopirovanej oblasti v pixloch
 * @param dst cielovy obrazok
 */
void copy_image_rect(
  const Image &src,
  int src_x,
  int src_y,
  int dst_x,
  int dst_y,
  int width,
  int height,
  Image &dst);

}
