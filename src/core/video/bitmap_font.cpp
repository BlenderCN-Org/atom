#include "core/video/bitmap_font.h"

#include <vector>
#include <limits>
#include <algorithm>
#include "core/system/resource_service.h"

using std::ifstream;
using std::swap;
using std::vector;
using std::numeric_limits;
using std::for_each;
using std::min;
using std::max;

namespace atom {

uptr<BitmapFont> BitmapFont::create(ResourceService &rs, const std::string &name)
{
  // load font definition from fnt file
  String fnt_filename = String(IMAGE_RESOURCE_DIR) + "/" + name + ".fnt";
  ifstream fnt_file(fnt_filename.c_str());

  if (!fnt_file.is_open()) {
    error("Can't read font fnt file for font \"%s\"", fnt_filename.c_str());
  }

  const int BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];

  // skip first 3 lines
  fnt_file.getline(buffer, BUFFER_SIZE);
  fnt_file.getline(buffer, BUFFER_SIZE);

  if (!fnt_file) {
    log::error("Error while reading FNT file header");
    return nullptr;
  }

  unsigned line_height, base_height, texture_width, texture_height;
  if (sscanf(buffer, "common lineHeight=%u base=%u scaleW=%u scaleH=%u",
        &line_height, &base_height, &texture_width, &texture_height) != 4) {
    log::error("Error while reading FNT file header - common section");
    return nullptr;
  }

  fnt_file.getline(buffer, BUFFER_SIZE);
  // read char count
  fnt_file.getline(buffer, BUFFER_SIZE);

  if (!fnt_file) {
    log::error("Error while reading FNT file header");
    return nullptr;
  }

  unsigned count = 0;
  if (sscanf(buffer, "chars count=%u", &count) != 1) {
    log::error("Error while reading FNT file header");
    return nullptr;
  }

  vector<CharInfo> chars;
  chars.reserve(count);

  for (unsigned i = 0; i < count; ++i) {
    if (!fnt_file.getline(buffer, BUFFER_SIZE)) {
      log::error("Error while reading FNT file \"%s\"", fnt_filename.c_str());
      return nullptr;
    }

    unsigned id, x, y, width, height, xadvance;
    int xoffset, yoffset;

    int read_count = sscanf(buffer,
      "char id=%u x=%u y=%u width=%u height=%u xoffset=%i yoffset=%i xadvance=%u",
      &id, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance);

    if (read_count != 8) {
      log::error("Error while reading char records from FNT file \"%s\"", fnt_filename.c_str());
      return nullptr;
    }
    // prepocitaj nacitane hodnoty do rozsahu 0.0 - 1.0 pre OpenGL texturu
    CharInfo info;
    info.id             = id;
    info.texture_x      = static_cast<float>(x)        / texture_width;
    info.texture_y      = static_cast<float>(y)        / texture_height;
    info.texture_width  = static_cast<float>(width)    / texture_width;
    info.texture_height = static_cast<float>(height)   / texture_height;
    info.width          = static_cast<float>(width)    / base_height;
    info.height         = static_cast<float>(height)   / base_height;
    info.xoffset        = static_cast<float>(xoffset)  / base_height;
    info.yoffset        = static_cast<float>(yoffset)  / base_height;
    info.xadvance       = static_cast<float>(xadvance) / base_height;
    info.aspect         = static_cast<float>(width)    / base_height;
    chars.push_back(info);
  }

  // nacitaj a spracuj kerning

  return uptr<BitmapFont>(new BitmapFont(rs.get_texture(name), chars,
    static_cast<float>(line_height) / base_height, 1));
}

Texture *BitmapFont::get_font_texture() const
{
  return my_texture->data();
}

BitmapFont::BitmapFont(
  const TextureResourcePtr &texture,
  const std::vector<CharInfo> &chars,
  float line_height,
  float base_height)
  : my_texture(texture)
  , my_min_char(numeric_limits<unsigned>::max())
  , my_max_char(0)
  , my_line_height(line_height)
  , my_base_height(base_height)
{
  assert(texture != nullptr);
  /// @todo nastavit zmysluplne hodnoty, napr. na prvy znak
  my_default_char.id             = 0;
  my_default_char.texture_x      = 0;
  my_default_char.texture_y      = 0;
  my_default_char.texture_width  = 0;
  my_default_char.texture_height = 0;
  my_default_char.xoffset        = 0;
  my_default_char.yoffset        = 0;
  my_default_char.xadvance       = 0;
  my_default_char.aspect         = 0;

  for (const CharInfo &info : chars) {
    my_min_char = min(info.id, my_min_char);
    my_max_char = max(info.id, my_max_char);
  }

  my_chars.resize(my_max_char - my_min_char + 1, my_default_char);

  for (const CharInfo &info : chars)
    my_chars[info.id - my_min_char] = info;
}

const CharInfo &BitmapFont::char_info(u8 c) const
{
  if (c >= my_min_char && c <= my_max_char)
    return my_chars[c - my_min_char];

  return my_default_char;
}

}
