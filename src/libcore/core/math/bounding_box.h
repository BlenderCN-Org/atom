#pragma once

#include "../platform.h"
#include "../math/mat.h"

namespace atom {

struct BoundingBox {
  f32 left;
  f32 top;
  f32 right;
  f32 bottom;

  BoundingBox();

  BoundingBox(f32 l, f32 t, f32 r, f32 b);

  static BoundingBox from_params(const Vec2f &pos, f32 w, f32 h);
};

bool contains(const BoundingBox &box, const Vec2f &point);

bool collide(const BoundingBox &a, const BoundingBox &b);

}
