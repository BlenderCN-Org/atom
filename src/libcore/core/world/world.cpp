#include "world.h"

#include <algorithm>
#include "update_context.h"
#include "../processor/render_processor.h"
#include "../processor/physics_processor.h"
#include "../processor/script_processor.h"
#include "../processor/debug_processor.h"

namespace atom {

sptr<World> World::create(Core &core)
{
  return sptr<World>(new World(core));
}

World::World(Core &core)
  : my_core(core)
  , my_is_live(false)
  , my_tick(0)
{
  init_processors();
}

World::~World()
{
  clear();
}

void World::add_entity(const sptr<Entity> &entity)
{
  assert(entity != nullptr);

  my_entities.push_back(entity);
  entity->activate();
}

void World::remove_entity(const sptr<Entity> &entity)
{
  assert(entity != nullptr);

  auto found = std::find(my_entities.begin(), my_entities.end(), entity);

  if (found == my_entities.end()) {
    log::warning("Entity not found");
    return;
  }

  entity->deactivate();
  my_entities.erase(found);
}

sptr<Entity> World::find_entity(const Vec2f &point) const
{
  not_implemented();
//  for (const sptr<Entity> &entity : my_entities) {
//    if (contains(entity->bounding_box(), point)) {
//      return entity;
//    }
//  }

  return nullptr;
}

sptr<Entity> World::find_entity(const String &id) const
{
  /// @todo pouzit binarne vyhladavanie
  auto found = std::find_if(my_entities.begin(), my_entities.end(),
    [&id](const sptr<Entity> &object) { return object->id() == id; });

  return found != my_entities.end() ? *found : nullptr;
}

void World::wake_up()
{
  my_is_live = true;
  my_processors.physics->start();
  my_processors.script->start();
  my_processors.debug->start();
}

void World::step()
{
  my_processors.physics->poll();
  my_processors.script->poll();
  my_processors.debug->poll();

  if (my_is_live) {
    ++my_tick;
  }
}

const EntityVector& World::objects() const
{
  return my_entities;
}

Slice<sptr<Entity> > World::all_entities() const
{
  return Slice<sptr<Entity>>(my_entities.data(), my_entities.size());
}

void World::clear()
{
  for (const sptr<Entity> &entity : my_entities) {
    entity->deactivate();
  }

  my_entities.clear();
}

sptr<World> World::clone() const
{
  sptr<World> world(new World(my_core));

  for (const sptr<Entity> &entity : my_entities) {
    sptr<Entity> duplicate = entity->clone(*world);

    if (duplicate == nullptr) {
      log::warning("Can't clone entity names \"%s\"", entity->id().c_str());
      continue;
    }

    world->add_entity(duplicate);
  }

  return world;
}

const WorldProcessorsRef& World::processors() const
{
  assert(my_processors_ref != nullptr);
  return *my_processors_ref;
}

f32 World::time() const
{
  return static_cast<f32>(my_tick) / FPS;
}

const Camera& World::camera() const
{
  return my_camera;
}

void World::set_camera(const Camera &camera)
{
  my_camera = camera;
}

void World::init_processors()
{
  my_processors.video.reset(new RenderProcessor(my_core.video_service(),
    my_core.resource_service()));
  my_processors.physics.reset(new PhysicsProcessor(my_core.video_service(),
    my_core.resource_service()));
  my_processors.script.reset(new ScriptProcessor());
  my_processors.debug.reset(new DebugProcessor(my_core.video_service(),
    my_core.resource_service(), *this));

  my_processors_ref.reset(new WorldProcessorsRef(*my_processors.video,
    *my_processors.physics, *my_processors.script, *my_processors.debug));
}

}
