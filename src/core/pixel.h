#pragma once

#include "platform.h"

namespace atom {

enum class PixelFormat {
  UNKNOWN,
  R,        ///< grayscale image 8bit
  RG,       ///< grayscale + alpha, 8bits per pixel, total 16bits
  RG16F,    ///< two 16bit float components (only texture format)
  RGB,      ///< rgb image, 8bits per pixel, total 24bits
  RGBA,     ///< rgba image, 8bits per pixel, total 32bits
  BGRA,     ///< same as rgba but reversed components
  RGBA32F,  ///< rgba float, 32bits per pixel (float), total 128bits (16bytes)
  RGBA32I,  ///< rgba, 32bits per pixel (signed int), total 128bits (16bytes)
  RGBA32UI, ///< rgba, 32bits per pixel (unsigned int), total 128bits (16bytes)
  RGB32I,   ///< rgb, 32bits per pixel (signed int), total 96bits (12bytes)
  R32I,     ///< grayscale, 32bits (signed int), total 32bits (4bytes)
  RGB32F,
  D16,
  D32,
  D24S8,
  D32F
};

struct PixelRGBA;
struct PixelR32I;
struct PixelBGRA;
struct PixelD24S8;

/**
 * Red (Alpha) format.
 */
struct PixelR {
  static const PixelFormat pixel_format = PixelFormat::R;

  u8 r;
};

static_assert(sizeof(PixelR) == 1, "Invalid struct size, bad data alignment");

/**
 * Red Green (Color Alpha) format.
 */
struct PixelRG {
  static const PixelFormat pixel_format = PixelFormat::RG;

  u8 r;
  u8 g;
};

static_assert(sizeof(PixelRG) == 2, "Invalid struct size, bad data alignment");

/**
 * Red Green Blue format.
 */
#pragma pack(push, 1)
struct PixelRGB {
  static const PixelFormat pixel_format = PixelFormat::RGB;

  u8 r;
  u8 g;
  u8 b;

  PixelRGB& operator=(const PixelRGBA &rgba);
};
#pragma pack(pop)

static_assert(sizeof(PixelRGB) == 3, "Invalid struct size, bad data alignment");

/**
 * Red Green Blue Alpha format.
 */
struct PixelRGBA {
  static const PixelFormat pixel_format = PixelFormat::RGBA;

  u8 r;
  u8 g;
  u8 b;
  u8 a;

  PixelRGBA() {}
//  PixelRGBA(const PixelRGBA &) = default;

  PixelRGBA(u8 red, u8 green, u8 blue, u8 alpha) :
      r(red), g(green), b(blue), a(alpha)
  {}

//  PixelRGBA& operator=(const PixelRGBA &rgba) = default;
  PixelRGBA& operator=(const PixelRGB &rgb);
  PixelRGBA& operator=(const PixelR32I &r32i);
  PixelRGBA& operator=(const PixelBGRA &bgra);
  PixelRGBA& operator=(const PixelD24S8 &d24s8);
};

static_assert(sizeof(PixelRGBA) == 4, "Invalid struct size, bad data alignment");

/**
 * Blue Green Red Alpha format.
 */
struct PixelBGRA {
  static const PixelFormat pixel_format = PixelFormat::BGRA;

  u8 b;
  u8 g;
  u8 r;
  u8 a;

//  PixelBGRA(const PixelBGRA &) ;
//  PixelBGRA& operator=(const PixelBGRA &) = default;

  /**
   * Konverzna funkcia z RGBA formatu
   */
  PixelBGRA& operator=(const PixelRGBA& rgba);
  PixelBGRA& operator=(const PixelRGB& rgb);
};

static_assert(sizeof(PixelBGRA) == 4, "Invalid struct size, bad data alignment");

/**
 * Red Green Blue 32bit integer per component
 */
struct PixelRGB32I {
  static const PixelFormat pixel_format = PixelFormat::RGB32I;

  int32_t r;
  int32_t g;
  int32_t b;
};

/**
 * Red Green Blue Alpha 32bit unsigned integer per component
 */
struct PixelRGBA32UI {
  static const PixelFormat pixel_format = PixelFormat::RGBA32UI;

  uint32_t r;
  uint32_t g;
  uint32_t b;
  uint32_t a;
};

/**
 * Red Green Blue Alpha 32bit float per component
 */
struct PixelRGBA32F {
  static const PixelFormat pixel_format = PixelFormat::RGBA32F;

  f32 r;
  f32 g;
  f32 b;
  f32 a;
};
static_assert(sizeof(PixelRGBA32F) == 16, "PixelRGBA32F ma velkost 16bytov (4 * 4bajty)");

/**
 * Red Green Blue 32bit unsigned integer per component.
 */
struct PixelRGB32F {
  static const PixelFormat pixel_format = PixelFormat::RGB32F;

  f32 r;
  f32 g;
  f32 b;
};
static_assert(sizeof(PixelRGB32F) == 12, "PixelRGB32F ma velkost 12bytov (3 * 4bajty)");

/**
 * Red 32bit integer format
 */
struct PixelR32I {
  static const PixelFormat pixel_format = PixelFormat::R32I;

  int32_t r;
};

static_assert(sizeof(PixelR32I) == 4, "Invalid struct size, bad data alignment");

/**
 * Depth 16bit format
 */
struct PixelD16 {
  static const PixelFormat pixel_format = PixelFormat::D16;

  u16 depth;
};

static_assert(sizeof(PixelD16) == 2, "Invalid struct size, must be 16bites");

/**
 * Depth 32bit format.
 */
struct PixelD32 {
  static const PixelFormat pixel_format = PixelFormat::D32;

  uint32_t depth; ///@todo myslim ze depth je 16bitovy float
};

static_assert(sizeof(PixelD32) == 4, "Invalid struct size, must be 32bites");

/**
 * Depth 24bit and Stencil 8bit format.
 */
struct PixelD24S8 {
  static const PixelFormat pixel_format = PixelFormat::D24S8;

  u8 stencil;
  u8 depth[3];
};

static_assert(sizeof(PixelD24S8) == 4, "Invalid struct size, must be 32bites");

/**
 * Depth 32bit float format.
 */
struct PixelD32F {
  static const PixelFormat pixel_format = PixelFormat::D32F;

  f32 depth; ///@todo myslim ze depth je 16bitovy float
};

static_assert(sizeof(PixelD32F) == 4, "Invalid struct size, must be 32bites");

//
// Konverzne funkcie: RGB->RGBA, RGBA->BGRA, ...
//

/**
 * Konverzna funkcia RGBA->RGB
 */
inline PixelRGB& PixelRGB::operator=(const PixelRGBA &rgba)
{
  r = rgba.r;
  g = rgba.g;
  b = rgba.b;
  return *this;
}

/**
 * Konverzna funkcia RGBA->BGRA
 */
inline PixelBGRA& PixelBGRA::operator=(const PixelRGBA& rgba)
{
  r = rgba.r;
  g = rgba.g;
  b = rgba.b;
  a = rgba.a;
  return *this;
}

/**
 * Konverzna funkcia RGB->BGRA
 */
inline PixelBGRA& PixelBGRA::operator=(const PixelRGB &rgb)
{
  r = rgb.r;
  g = rgb.g;
  b = rgb.b;
  a = 255;
  return *this;
}

/**
 * Konverzna funkcia RGB->RGBA
 */
inline PixelRGBA& PixelRGBA::operator=(const PixelRGB &rgb)
{
  r = rgb.r;
  g = rgb.g;
  b = rgb.b;
  a = 255;
  return *this;
}

/**
 * Konverzna funkcia R32I->RGBA
 */
inline PixelRGBA& PixelRGBA::operator=(const PixelR32I &r32i)
{
  if (r32i.r < 0) {
    r = g = b = a = 0;
  } else {
    const PixelRGBA *rgba = reinterpret_cast<const PixelRGBA *>(&r32i);
//        r = g = b = r32i.r;
    r = rgba->r;
    g = rgba->g;
    b = rgba->b;
    a = 255;
  }
  return *this;
}

/**
 * Konverzna funkcia BGRA->RGBA
 */
inline PixelRGBA& PixelRGBA::operator=(const PixelBGRA &bgra)
{
  r = bgra.r;
  g = bgra.g;
  b = bgra.b;
  a = bgra.a;
  return *this;
}

/**
 * Konverzna funkcia D24S8->RGBA
 */
inline PixelRGBA& PixelRGBA::operator=(const PixelD24S8 &d24s8)
{
  r = d24s8.depth[0];
  g = d24s8.depth[1];
  b = d24s8.depth[2];
  a = d24s8.stencil;
  return *this;
}

}
