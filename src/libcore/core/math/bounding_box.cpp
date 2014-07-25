#include "bounding_box.h"

namespace atom {

BoundingBox::BoundingBox()
  : left(0)
  , top(0)
  , right(0)
  , bottom(0)
{
}

BoundingBox::BoundingBox(f32 l, f32 t, f32 r, f32 b)
  : left(l)
  , top(t)
  , right(r)
  , bottom(b)
{
}

BoundingBox BoundingBox::from_params(const Vec2f &pos, f32 w, f32 h)
{
  return BoundingBox(pos.x - w / 2, pos.y + h / 2, pos.x + w / 2, pos.y - h / 2);
}

bool contains(const BoundingBox &box, const Vec2f &point)
{
  return (point.x >= box.left) && (point.x <= box.right) &&
         (point.y >= box.bottom) && (point.y <= box.top);
}

bool collide(const BoundingBox &a, const BoundingBox &b)
{
  if (a.left > b.right || a.top < b.bottom || a.right < b.left || a.bottom > b.top) {
    return false;
  }

  return true;
}

}
