#pragma once

#include "vec.h"

namespace atom {

template<typename T>
struct AxisAngle {
  Vec3<T> axis;
  T angle;
};

typedef AxisAngle<float> AxisAnglef;

}
