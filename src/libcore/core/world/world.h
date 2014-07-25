#pragma once

#include "entity.h"
#include "../corefwd.h"
#include "../math/mat.h"
#include "../math/camera.h"
#include "../video/mesh_tree.h"

namespace atom {

class Runnable {
  virtual ~Runnable() = default;

  virtual void run() = 0;
};

typedef std::vector<sptr<Entity>> EntityVector;

/// processory pre jednotlive komponenty
struct WorldProcessors {
  uptr<VideoProcessor>   video;
  uptr<PhysicsProcessor> physics;
  uptr<ScriptProcessor> script;
};

/// referencie na processory, umoznuju pohodlny pristup pomocou jednej metody processors()
struct WorldProcessorsRef {
  VideoProcessor   &video;
  PhysicsProcessor &physics;
  ScriptProcessor &script;

  WorldProcessorsRef(VideoProcessor &vp, PhysicsProcessor &pp, ScriptProcessor &sp)
    : video(vp)
    , physics(pp)
    , script(sp)
  {
    // empty
  }
};

class World {
public:
  static sptr<World> create(Core &core);

  World(Core &core);
  ~World();

  void add_entity(const sptr<Entity> &object);

  void remove_entity(const sptr<Entity> &entity);

  sptr<Entity> find_entity(const Vec2f &point) const;

  sptr<Entity> find_entity(const String &id) const;

  void wake_up();

  /// vykonaj jeden simulacny krok (1 krok = 1s / FPS)
  void step();

//  MeshTree& mesh_tree();

  const EntityVector& objects() const;

  void clear();

  sptr<World> clone() const;

  const WorldProcessorsRef& processors() const;

  f32 time() const;

  const Camera& camera() const;

  void set_camera(const Camera &camera);

  void debug_draw();

  void post(sptr<Runnable> runnable);

  void post_delayed(sptr<Runnable> runnable, double delay);

  void post_at_time(sptr<Runnable> runnable, double time);

private:
  /**
   * Inicializuj jednotlive procesory (a inicializuj referencie na ne).
   */
  void init_processors();

private:
  Core                     &my_core;
  bool                      my_is_live;
  uint64_t                  my_tick;
  Camera                    my_camera;
  WorldProcessors           my_processors;
  uptr<WorldProcessorsRef>  my_processors_ref;
  std::vector<sptr<Entity>> my_entities;
};

}
