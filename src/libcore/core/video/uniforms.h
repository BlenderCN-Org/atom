#pragma once

#include "../corefwd.h"
#include "../meta.h"
#include "../math/transformations.h"
#include "../math/mat_array.h"

namespace atom {

class Uniforms {
public:
  META_DECLARE_CLASS_PTR;
  META_DECLARE_CLASS;

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
  Mat4f mvp;
  Mat4f model;
  Vec3f sun_dir;
  Mat4fArray bones;
  Transformations transformations;
};

}
