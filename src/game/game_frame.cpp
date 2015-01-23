#include "game_frame.h"
#include <core/world.h>
#include <fstream>
#include <core/json_utils.h>
#include <core/input_service.h>
#include <core/gbuffer.h>
#include <core/render_processor.h>
#include <core/resource_service.h>
#include <core/debug_processor.h>

namespace atom {

namespace {

sptr<Entity> create_entity(const std::vector<EntityDefinition> &creators, const String &class_name,
  World &world, Core &core)
{
  auto found = std::find_if(creators.begin(), creators.end(),
    [&class_name](const EntityDefinition &creator) { return creator.name == class_name; });

  if (found == creators.end()) {
    return nullptr;
  }

  sptr<Entity> entity = found->create(world, core);
  entity->set_class_name(class_name);
  return entity;
}

bool load_level_from_file(const String &filename, Core &core, World &world)
{
  world.clear();

  std::ifstream input(filename);

  if (!input.is_open()) {
    log_warning("Can't open file \"%s\"", filename.c_str());
    return false;
  }

  utils::JsonInputStream stream(input);
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);

  if (doc.HasParseError()) {
    log_error("%s Offset %i", doc.GetParseError(), doc.GetErrorOffset());
    return false;
  }

  if (!doc.IsObject()) {
    log_error("Top level element must be object");
    return false;
  }

  if (!doc.HasMember("entities")) {
    log_error("Level file doesn't contain entities");
    return false;
  }

  const rapidjson::Value &entities = doc["entities"];

  u32 count = entities.Size();

  for (uint i = 0; i < count; ++i) {
    const rapidjson::Value &obj = entities[i];
    // each entity must be object
    if (!obj.IsObject()) {
      log_error("Entity must be object, skipping");
      continue;
    }
    // each entity class is identified by "class" field
    if (!obj.HasMember("class")) {
      log_error("Entity missing \"class\" field, skipping");
      continue;
    }

    const rapidjson::Value &class_name = obj["class"];

    if (!class_name.IsString()) {
      log_error("Entity \"class\" must be string, skipping");
      continue;
    }

    const String entity_class(class_name.GetString());

    sptr<Entity> entity = create_entity(core.entity_creators(), entity_class, world, core);

    if (entity == nullptr) {
      log_warning("Unknown entity class \"%s\"", entity_class.c_str());
      continue;
    }

    // load properties
    MetaFieldsEnumarator enumerator(*entity->meta);
    const MetaField *field;

    while ((field = enumerator.next())) {
      utils::ReadResult result = utils::read_basic_property_from_json(obj, *field, entity.get());

      if (result != utils::ReadResult::OK) {
        log_warning("Can't read field \"%s\" of entity \"%s\"", field->name, entity_class.c_str());
      }
    }

    world.add_entity(entity);
  }

  return true;
}




void process_sdl_events(InputService &is)
{
  SDL_Event event;
  // spracuj/preposli jednotlive event-y
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_KEYUP:
      case SDL_KEYDOWN:
//        if (!event.key.repeat) {
        is.push_event(make_key_event(sdl_key_to_key(event.key.keysym.sym),
          event.type == SDL_KEYDOWN));
//        }
        break;

      case SDL_JOYAXISMOTION:
//        if (event.jaxis.axis < to_size(Axis::COUNT)) {
//          is.push_event(make_axis_event(Axis(event.jaxis.axis), event.jaxis.value / 32768.0));
//        }
        break;

      /// @todo mouse events
      case SDL_MOUSEMOTION:
        break;

      case SDL_MOUSEBUTTONDOWN:
        break;

      case SDL_MOUSEBUTTONUP:
        break;

      /// @todo joystick events
//      case SDL_JOYBUTTONUP:
//      case SDL_JOYBUTTONDOWN:
//      case SDL_JOYBALLMOTION:
//      case SDL_JOYHATMOTION:
//        info("Joystick event");
//        break;
    }
  }
}

}


GameFrame::GameFrame(Core &core, const String &level_name)
  : Frame(core)
{
  my_world.reset(new World(core));
  if (!load_level_from_file(level_name, core, *my_world)) {
    error("Can't load level \"%s\"", level_name.c_str());
    return;
  }
  my_world->activate();
}

GameFrame::~GameFrame()
{
  my_world->deactivate();
}

void GameFrame::input()
{
  InputService &is = core().input_service();
  if (is.is_key_down(Key::KEY_Q) &&
    (is.is_key_down(Key::KEY_LCTRL) || is.is_key_down(Key::KEY_RCTRL))) {
    exit_frame();
  }
}

void GameFrame::update()
{
  process_sdl_events(core().input_service());
  core().update();
  my_world->tick();
}

void GameFrame::draw()
{
  const Config &config = Config::instance();
  my_world->processors().video.set_resolution(config.get_screen_width(), config.get_screen_height());
  GBuffer &gbuffer = my_world->processors().video.get_gbuffer();

  core().video_service().bind_write_framebuffer(gbuffer.get_render_framebuffer());

  if (my_world == nullptr) {
    return;
  }

  core().resource_service().poll();

  glClearColor(0.09, 0.17, 0.27, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  my_world->processors().video.render(my_world->camera());

  my_world->processors().debug.draw();

  core().video_service().unbind_write_framebuffer();
  my_world->processors().video.get_gbuffer().blit();
}

}
