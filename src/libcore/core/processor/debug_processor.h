#pragma once

#include "../foundation.h"
#include "processor.h"

namespace atom {

enum class DebugCategory {
  PHYSICS = 1,
  BOUNDING_BOX = 2,
  AABB = 4
};

class PhysicsDebugDrawer;

class DebugProcessor : public Processor {
  VideoService            &my_vs;
  World                   &my_world;
  u32                      my_debug_categories;
  uptr<PhysicsDebugDrawer> my_physics_drawer;
  MaterialResourcePtr      my_wireframe_material;
  MaterialResourcePtr      my_bounding_box_material;
  MaterialResourcePtr      my_aabb_material;

public:
  DebugProcessor(VideoService &vs, ResourceService &rs, World &world);
  ~DebugProcessor();

  void poll() override;

  void start() override;

  void draw();

  void set_debug(DebugCategory category, bool enable);


private:
  void draw_physics();
  void draw_bounding_box();
  void draw_aabb();
};

}
