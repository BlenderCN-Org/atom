#include "sdl.h"

#include "../log.h"
#include "../utils/utils.h"
#include "../utils/gl_utils.h"
#include "config.h"

namespace atom {

SDL::SDL()
  : my_is_initialized(false)
{
  init_video();

  my_is_initialized = true;
}

SDL::~SDL()
{
  if (is_initialized()) {
    GL_CHECK_ERROR;
    SDL_Quit();
  }
}

void SDL::init_video()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
    error("Can't initialize SDL: %s", SDL_GetError());
  }
  // set OpenGL parameters
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE  , 24);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE    , 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE  , 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE   , 8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE  , 8);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  // load screen parameters from config
  Config &config = Config::instance();
  int width  = config.get_screen_width();
  int height = config.get_screen_height();
  int bpp    = config.get_screen_bits_per_pixel();
  // set video mode
  if (SDL_SetVideoMode(width, height, bpp, SDL_OPENGL) == 0) {
    error("Can't set video mode %ix%ix%i: %s", width, height, bpp, SDL_GetError());
  }
  log::info("Video mode %ix%ix%i", width, height, bpp);
  // initialize GLEW
  GLenum result = glewInit();
  if (result != GLEW_OK) {
    error("Can't initialize GLEW\n", glewGetErrorString(result));
  }
  // disable software key repeat
  SDL_EnableKeyRepeat(0, 0);
}

void SDL::flip()
{
  SDL_GL_SwapBuffers();
}

void SDL::update_input()
{
  SDL_JoystickUpdate();
}

}
