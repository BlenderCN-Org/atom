#pragma once

#include "../foundation.h"
#include "../constants.h"
#include "../utils/singleton.h"
#include "../system/core.h"

namespace atom {

// predbezna deklaracia

class Config : public utils::Singleton<Config> {
  friend class Core;

public:
  META_ROOT_CLASS;
  
  // public methods
  /**
   * Vrat objekt s nacitanym nastavenim.
   */
  static Config& instance()
  {
    return Singleton<Config>::instance();
  }

  int get_screen_width() const
  { return screen_width; }

  int get_screen_height() const
  { return screen_height; }

  int get_screen_bits_per_pixel() const
  { return screen_bpp; }

  float get_aspect_ratio() const
  { return static_cast<float>(get_screen_width()) / get_screen_height(); }

  int get_index_texture_width() const
  { return screen_width / 2; }

  int get_index_texture_height() const
  { return screen_height / 2; }

  void set_screen_resolution(u32 width, u32 height);

//  bool debug_inotify() const
//  { return debug_inotify; }

  bool get_debug() const
  { return debug; }

//  bool debug_video() const
//  { return debug_video; }

//  bool debug_input() const
//  { return debug_input; }

//  bool debug_resources() const
//  { return debug_resources; }

//  bool debug_counters() const
//  { return debug_counters; }

private:
  // private methods
  Config();

  static void create();

  void load_config_file();

  void load_environment();

public:
  bool color_log;
  bool debug_inotify;
  bool debug;          ///< zatial nema vyuzitie
  bool debug_video;
  bool debug_audio;
  bool debug_input;
  bool debug_resources;
  bool debug_counters;

private:
  int screen_width;
  int screen_height;
  int screen_bpp;

  
};

}
