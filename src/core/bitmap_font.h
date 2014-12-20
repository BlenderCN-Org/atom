#pragma once

#include <fstream>
#include "texture.h"

namespace atom {

struct CharInfo {
  unsigned id;
  float texture_x;
  float texture_y;
  float texture_width;
  float texture_height;
  float width;
  float height;
  float xoffset;
  float yoffset;
  float xadvance;
  float aspect;   ///< pomer
};

class BitmapFont : private NonCopyable {
public:
  static uptr<BitmapFont> create(
    ResourceService &rs,
    const std::string &name);

  Texture* get_font_texture() const;

  const CharInfo& char_info(u8 c) const;

  float line_height() const
  { return my_line_height; }

  float base_height() const
  { return my_base_height; }

private:
  BitmapFont(
    const TextureResourcePtr &texture,
    const std::vector<CharInfo> &chars,
    float line_height,
    float base_height);

private:
  TextureResourcePtr    my_texture; ///< font texture
  u32                   my_min_char;
  u32                   my_max_char;
  float                 my_line_height;
  float                 my_base_height;
  CharInfo              my_default_char;
  std::vector<CharInfo> my_chars;         ///< informacie o znakoch
};

}
