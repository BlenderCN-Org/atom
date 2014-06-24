#include "core/math/camera.h"
#include "core/utils/utils.h"

namespace atom {
namespace {

const Vec3f our_up = Vec3f::axis_z();
const Vec3f our_right = Vec3f::axis_x();
const Vec3f our_forward = Vec3f::axis_y();

}

Mat4f calculate_basic_view(const Vec3f &pos, f32 yaw, f32 pitch)
{
  Mat4f rotation = Mat4f::rotation_z(yaw) * Mat4f::rotation_x(-pitch);
  return Mat4f::rotation_x(M_PI / 2) * rotation * Mat4f::translation(pos);
}

//Camera Camera::perspective(
//  float fov,
//  float aspect,
//  float near,
//  float far)
//{
//  Camera camera;
//  camera.set_perspective(fov, aspect, near, far);
//  return camera;
//}

//Camera::Camera()
//  : my_type(Type::PERSPECTIVE)
//  , my_near(0.1)
//  , my_far(100)
//  , my_fov(1.57)
//  , my_aspect(16.0 / 9.0)
//  , my_yaw(0)
//  , my_pitch(0)
//  , my_roll(0)
//  , my_position(Vec3f(0, 0, 0))
//{
//}

//void Camera::set_orthographic(
//  float xmin,
//  float xmax,
//  float ymin,
//  float ymax,
//  float near,
//  float far)
//{
//  assert(far > near);

//  my_type       = Type::ORTHOGRAPHIC;
//  my_xmin       = xmin;
//  my_xmax       = xmax;
//  my_ymin       = ymin;
//  my_ymax       = ymax;
//  my_near       = near;
//  my_far        = far;
//  my_projection = Mat4f::orthographic(xmin, xmax, ymin, ymax, near, far);
//}

//void Camera::set_perspective(float fov, float aspect, float near, float far)
//{
//  assert(fov > 0.0);
//  assert(far > near);
//  assert(aspect > 0.0);

//  my_type       = Type::PERSPECTIVE;
//  my_fov        = fov;
//  my_aspect     = aspect;
//  my_near       = near;
//  my_far        = far;
//  my_projection = Mat4f::perspective(fov, aspect, near, far);
//}

//void Camera::set_rotation(float yaw, float pitch, float roll)
//{
//  my_yaw = roll;
//  my_pitch = pitch;
//  my_roll = yaw;
//}

//void Camera::rotate_camera(float yaw, float pitch, float roll)
//{
//  my_yaw += yaw;
//  my_pitch += pitch;
//  my_roll += roll;
//}

//Mat4f Camera::view_matrix() const
//{
////  Quatf full_rotation = calculate_rotation() * Quatf::from_axis_angle(Vec3f::axis_x(), PI / 2);
////  return Mat4f::translation(my_position) * full_rotation.rotation_matrix();
//  return (Mat4f::translation(my_position) * calculate_rotation().rotation_matrix()).inverted();
//}

//Mat4f Camera::projection_matrix() const
//{
//  return my_projection;
//}

//Mat4f Camera::infinite_perspective_projection_matrix() const
//{
//  assert(my_type == Type::PERSPECTIVE);
//  return Mat4f::infinite_perspective(my_fov, my_aspect, my_near);
//}

//Mat4f Camera::rotation_matrix() const
//{
//  return calculate_rotation().rotation_matrix();
//}

//Vec3f Camera::up() const
//{
//  return rotate(calculate_rotation(), our_up);
//}

//Vec3f Camera::forward() const
//{
//  return rotate(calculate_rotation(), our_forward);
//}

//Vec3f Camera::right() const
//{
//  return rotate(calculate_rotation(), our_right);
//}

//Quatf Camera::calculate_rotation() const
//{
//  Quatf yaw = Quatf::from_axis_angle(our_up, my_yaw);
//  Quatf pitch = Quatf::from_axis_angle(rotate(yaw, our_right), my_pitch);
//  Quatf roll = Quatf::from_axis_angle(rotate(yaw, our_forward), my_roll);

//  return roll * pitch * yaw;
//}

}
