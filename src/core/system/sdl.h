#pragma once

#include <SDL/SDL.h>
#include "core/corefwd.h"
#include "core/input/mouse.h"

namespace atom {

class SDL {
public: // public methods
  /**
   * @todo zabezpecit vytvorenie len jednej instancie
   */
  SDL();
  ~SDL();

  bool is_initialized() const
  { return my_is_initialized; }

  void flip();

//  void set_capture_mouse(bool capture);

  /**
   * Spracuj vstup, aktualizuj stav gamepad-ov a mysi, ...
   */
  void update_input();

private:
  // private methods

  void init_video();

private:
  // private members
  bool           my_is_initialized;
//  SDL_Window    *my_sdl_window;
//  SDL_GLContext  my_sdl_context;
};

}
