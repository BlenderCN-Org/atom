#pragma once

#include "../foundation.h"
#include "processor.h"

namespace atom {

namespace DebugCategory {
  enum {
    PHYSICS = 1,
    BOUNDING_BOX = 2,
    AABB = 4
  };
}

class PhysicsDebugDrawer;

class DebugProcessor : public NullProcessor {
  u32                      my_debug_categories;
  uptr<PhysicsDebugDrawer> my_physics_drawer;
  MaterialResourcePtr      my_wireframe_material;
  MaterialResourcePtr      my_bounding_box_material;
  MaterialResourcePtr      my_aabb_material;

public:
  explicit DebugProcessor(World &world);
  ~DebugProcessor();

  void activate() override;

  void poll() override;

  void draw();

  void set_debug(u32 category, bool enable);


private:
  void draw_physics();
  void draw_bounding_box();
  void draw_aabb();
};

}
