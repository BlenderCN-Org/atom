#pragma once

#include "core/math/transformations.h"
#include "core/math/mat_array.h"

namespace atom {

class Uniforms {
public:
  Uniforms();

  Vec3f color;
  Vec3f foreground_color;
  Vec3f background_color;
  Vec3f ambient_color;
  Vec3f diffuse_color;
  Vec3f specular_color;
  float alpha;
  Vec3f light_position;
  Vec3f eye_direction;
  Transformations transformations;
};

}
