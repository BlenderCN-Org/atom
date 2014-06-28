#pragma once

#include "core/math/mat.h"
#include "core/math/quaternion.h"
#include "core/log.h"

namespace atom {

const Vec3f CAMERA_UP = Vec3f::axis_z();
const Vec3f CAMERA_RIGHT = Vec3f::axis_x();
const Vec3f CAMERA_FRONT = Vec3f::axis_y();

struct Camera {
  Mat4f view;
  Mat4f projection;
};


class BasicCamera {
  Vec3f my_position;
  f32   my_yaw;
  f32   my_pitch;

public:
  BasicCamera();

  Vec3f get_position() const
  {
    return my_position;
  }

  void set_position(const Vec3f &pos)
  {
    my_position = pos;
  }

  void set_yaw(f32 yaw)
  {
    my_yaw = yaw;
  }

  void set_pitch(f32 pitch)
  {
    my_pitch = pitch;
  }

  f32 get_yaw() const
  {
    return my_yaw;
  }

  f32 get_pitch() const
  {
    return my_pitch;
  }

  Mat4f get_view_matrix() const;

  Vec3f get_front() const;

  Vec3f get_up() const;

  Vec3f get_right() const;

private:
  Quatf get_rotation() const;

  Mat4f get_rotationm() const;
};

typedef sptr<Camera> CameraPtr;

}
