#include "camera.h"
#include "../utils/utils.h"

namespace atom {

const Mat4f CAMERA_NORMALIZE = Mat4f::rotation_x(M_PI / 2);
const Mat4f CAMERA_DENORMALIZE = CAMERA_NORMALIZE.inverted();

// kamera je v bode [0, 0, 0] a pozera sa smerom [0, 1, 0]
//Mat4f calculate_basic_view(const Vec3f &pos, f32 yaw, f32 pitch)
//{
//  Quatf rot = Quatf::from_axis_angle(Vec3f::axis_x(), pitch) * Quatf::from_axis_angle(Vec3f::axis_z(), -yaw);
//  Mat4f m = Mat4f::translation(pos) * rot.rotation_matrix() * CAMERA_NORMALIZE;
//  return m.inverted();
//}

BasicCamera::BasicCamera()
  : my_position(0, 0, 0)
  , my_yaw(0)
  , my_pitch(0)
{
}

Mat4f BasicCamera::get_view_matrix() const
{
  Quatf r = get_rotation() * Quatf::from_axis_angle(Vec3f::x_axis(), M_PI / 2);
  Mat4f m = Mat4f::translation(my_position)
          * r.rotation_matrix();
  return m.inverted();
}

Vec3f BasicCamera::get_front() const
{
  return rotate(get_rotation(), Vec3f(0, 1, 0));
}

Vec3f BasicCamera::get_up() const
{
  return rotate(get_rotation(), Vec3f(0, 0, 1));
}

Vec3f BasicCamera::get_right() const
{
  return rotate(get_rotation(), Vec3f(1, 0, 0));
}

Quatf BasicCamera::get_rotation() const
{
  Quatf yaw = Quatf::from_axis_angle(Vec3f::z_axis(), -my_yaw);
  Quatf pitch = Quatf::from_axis_angle(rotate(yaw, Vec3f::x_axis()), my_pitch);
  return pitch * yaw;
}

Mat4f BasicCamera::get_rotationm() const
{
 return Mat4f::rotation_z(-my_yaw) * Mat4f::rotation_x(my_pitch);
}

}
