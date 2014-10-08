#include "core.h"

#ifdef __linux__
#include <signal.h>
#include <execinfo.h>
#endif

#include "../video/draw_service.h"
#include "../video/video_service.h"
#include "../audio/audio_service.h"
#include "../input/input_service.h"
#include "game_api.h"
#include "config.h"

namespace atom {

namespace {
uptr<Core> core_instance;
}

#ifdef __linux__
static sighandler_t previous_handler = nullptr;
#endif

void segmentation_fault_handler(int sig)
{
  fprintf(stderr, "Segmentation fault\n");
  const int BACKTRACE_SIZE = 16;
  void *array[BACKTRACE_SIZE];

#ifdef __linux__
  int size = backtrace(&array[0], BACKTRACE_SIZE);
  backtrace_symbols_fd(&array[0], size, 2);
  // call original signal handler
  if (previous_handler != nullptr)
    previous_handler(sig);
#endif

  exit(EXIT_FAILURE);
}

Core& Core::init(InitMode mode, const EntryPoint *entry_point)
{
  assert(core_instance == nullptr);
  core_instance.reset(new Core(entry_point));
  core_instance->do_init(mode);
  return *core_instance;
}

void Core::quit()
{
  assert(core_instance != nullptr);
  core_instance.reset();
}

void Core::update()
{
}

Core::Core(const EntryPoint *entry_point)
  : my_is_initialized(false)
{
  // registracia backtrace vypisu
#ifdef __linux
  previous_handler = signal(SIGSEGV, segmentation_fault_handler);
#endif

  log::info("Project Atom (build %s %s)", __DATE__, __TIME__);

  if (entry_point == nullptr) {
    error("Can't find game entry point");
    return ;
  }
  // load game object creators
  my_entity_creators = entry_point->make_entity_creators(*this);
}

Core::~Core()
{
  quit_services();
  SDL_Quit();
}

void Core::do_init(InitMode mode)
{
  //
  // Inicializacia Config
  //
  my_config.reset(new Config());
  Config::set_instance(my_config.get());

  //
  // Inicializacia VideoService
  //
  if (mode == InitMode::STANDALONE) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0) {
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

    // disable software key repeat
    SDL_EnableKeyRepeat(0, 0);
  } else {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
      error("Can't initialize SDL: %s", SDL_GetError());
    }
  }

  // initialize GLEW

  if (flextInit() != GL_TRUE) {
    error("Can't initialize Opengl 4.2");
    return;
  }

  init_services();

  my_is_initialized = true;
}

void Core::init_services()
{
  assert(my_is_initialized == false);

  my_services.video.reset(new VideoService());

  // Inicializacia InputService-u
  my_services.input.reset(new InputService());

  // Inicializacia ResourceService
  my_services.resource.reset(new ResourceService(*this));

  // Inicializacia DrawService
  my_services.draw.reset(new DrawService(video_service(), resource_service()));

  // Inicializacia AudioService
  my_services.audio.reset(new AudioService());
}

void Core::quit_services()
{
  // remove order is important
  my_services.draw.reset();
  my_services.resource.reset();
  my_services.input.reset();
  my_services.video.reset();
  my_services.audio.reset();
}

void Core::unload_game_lib()
{
  my_entity_creators.clear();
}

const std::vector<EntityCreator> &Core::entity_creators() const
{
  return my_entity_creators;
}

}
