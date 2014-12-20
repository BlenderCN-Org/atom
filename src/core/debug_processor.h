#pragma once

#include "foundation.h"
#include "processor.h"
#include "stdvec.h"

namespace atom {

namespace DebugCategory {
  enum {
    PHYSICS        = 1,
    BOUNDING_BOX   = 2,
    AABB           = 4,
    GEOMETRY_CACHE = 8
  };
}

class PhysicsDebugDrawer;

class DebugProcessor : public NullProcessor {
  u32                      my_debug_categories;
  uptr<PhysicsDebugDrawer> my_physics_drawer;
  MaterialResourcePtr      my_debug_material;
  std::vector<Vec3f>       my_line_points;
  std::vector<Vec3f>       my_line_colors;

public:
  explicit DebugProcessor(World &world);
  ~DebugProcessor();

  void activate() override;

  void poll() override;

  void draw();

  void set_debug(u32 category, bool enable);

  void draw_line(const Vec3f &start, const Vec3f &end, const Vec3f &color);

  void draw_line(const Mat4f &transform, const Vec3f &start, const Vec3f &end,
    const Vec3f &color);

  void clear();

private:
  void gather_physics();
  void gather_bounding_box();
  void gather_aabb();
  void gather_geometry_cache();
  void draw_all();
};

}
