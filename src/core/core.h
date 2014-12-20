#pragma once

#include <cassert>
#include "corefwd.h"
#include "noncopyable.h"
#include "game_api.h"

namespace atom {

enum class InitMode {
  STANDALONE,
  EDITOR
};

/**
 * Riadena inicializacia singletonov/podsystemov.
 * Tato trieda riadi poradie vytvarania/rusenia vsetkych singletonov v aplikacii.
 * Existuje len jedina (staticka) instancia tejto triedy, co zarucuje len jednu
 * inicializaciu a jednu deinicializaciu ostatnych singletonov (podsystemov).
 * Singletony su vytvarane a inicializovane v konstruktore, rusia sa v destruktore.
 *
 * Poradie vytvarania (poradie rusenia je opacne):
 *   Config
 *   VideoService
 *   DrawService
 *   AudioService
 *   ResourceService
 */
class Core : private NonCopyable {
public:
  /**
   * Spusti proces inicializacie celeh prostredia, po skonceni tejto funkcie
   * je cele prostredie (VideoService, DrawService, AudioService,
   * ResourceService, ...) pripravene k pouzitiu. Vracia odkaz na instanciu,
   * ktora je nepotrebna, pretoze rozhranie God neobsahuje ziadne verejne
   * metody.
   *
   * @return referencia na instanciu God
   */
  static Core& init(InitMode mode, const EntryPoint *entry_point);
  static void quit();

  ~Core();

  VideoService& video_service() const
  {
    assert(my_services.video != nullptr);
    return *my_services.video;
  }

  InputService& input_service() const
  {
    assert(my_services.input != nullptr);
    return *my_services.input;
  }

  AudioService& audio_service() const
  {
    assert(my_services.audio != nullptr);
    return *my_services.audio;
  }

  ResourceService& resource_service() const
  {
    assert(my_services.resource != nullptr);
    return *my_services.resource;
  }

  void update();

  bool load_game_lib(const char *name);
  void unload_game_lib();

  const std::vector<EntityCreator>& entity_creators() const;

private:
  struct Services {
    uptr<InputService>    input;
    uptr<VideoService>    video;
    uptr<AudioService>    audio;
    uptr<ResourceService> resource;
  };

  /**
   * Inicializuje vsetky singletony/podsystemy.
   */
  Core(const EntryPoint *entry_point);

  void do_init(InitMode mode);

  void init_services();

  void quit_services();

  /// @todo klasicke ukazovatele prepisat na unique_ptr
  bool              my_is_initialized;
  uptr<Config>      my_config;
  Services          my_services;
  std::vector<EntityCreator> my_entity_creators;
};

}
