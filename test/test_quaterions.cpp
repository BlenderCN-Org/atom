#include <gtest/gtest.h>
#include <core/utils.h>
#include "test_utils.h"

namespace atom {

const Vec3f AXIS_X = Vec3f::x_axis();
const Vec3f AXIS_Y = Vec3f::y_axis();
const Vec3f AXIS_Z = Vec3f::z_axis();

TEST(QuaternionTest, ConstructorAndSetOperation)
{
  Quatf q(1.0f, 2.0f, 3.0f, 4.0f);

  EXPECT_QUATF_EQ(Quatf(1, 2, 3, 4), q)

  q.w = -5.0f;
  q.x = -6.0f;
  q.y = -7.0f;
  q.z = -8.0f;

  EXPECT_QUATF_EQ(Quatf(-5.0f, -6.0f, -7.0f, -8.0f), q);

  Quatf a(2.0f, 4.0f, 8.0f, 16.0f);
  q = Quatf(2.0f, 4.0f, 8.0f, 16.0f);
  EXPECT_QUATF_EQ(Quatf(2.0f, 4.0f, 8.0f, 16.0f), a);
  EXPECT_EQ(a, q);
}

TEST(QuaternionTest, Conjugate)
{
  Quatf q(1.0f, 2.0f, 3.0f, 4.0f);
  q.conjugated();
  EXPECT_QUATF_EQ(Quatf(1.0f, 2.0f, 3.0f, 4.0f), q);
  EXPECT_QUATF_EQ(Quatf(1.0f, -2.0f, -3.0f, -4.0f), q.conjugated());
  EXPECT_QUATF_EQ(Quatf(1.0f, -2.0f, -3.0f, -4.0f), q.conjugated());  // braces are required here (because of gtest)
}

TEST(QuaternionTest, Length)
{
  Quatf a(1.0f, 1.0f, 1.0f, 1.0f);
  EXPECT_EQ(2.0f, a.length());

  Quatf b(2.0f, -5.0f, 9.9f, -1.0f);
  EXPECT_FLOAT_EQ(11.314150432f, b.length());
}

TEST(QuaternionTest, Length2)
{
  Quatf a(1.0f, 1.0f, 1.0f, 1.0f);
  EXPECT_EQ(4.0f, a.length2());

  Quatf b(2.0f, -5.0f, 9.9f, -1.0f);
  EXPECT_EQ(128.01f, b.length2());
}

TEST(QuaternionTest, Inversion)
{
  Quatf a(1, 2, 3, 4);
  a.inverted();
  EXPECT_QUATF_EQ(Quatf(1.0, 2.0, 3.0, 4.0), a);
  a = a.inverted();
  EXPECT_QUATF_EQ(Quatf(1.0f/30.0f, -2.0f/30.0f, -3.0f/30.0f, -4.0f/30.0f), a);
}

TEST(QuaternionTest, Normalization)
{
  Quatf a(5, 3, -2, 2);
  a.normalized();
  EXPECT_QUATF_EQ(Quatf(5.0, 3.0, -2.0, 2.0), a);
  a = a.normalized();
  EXPECT_QUATF_EQ(Quatf(0.771516749f, 0.462910049f, -0.308606699f, 0.308606699f), a);
}

TEST(QuaternionTest, Addition)
{
  Quatf a(1, 2, 3, 4);
  Quatf b(-1, -2, -3, -4);
  Quatf c = a + b;
  EXPECT_QUATF_EQ(Quatf(0, 0, 0, 0), c);
}

TEST(QuaternionTest, Identity)
{
  EXPECT_QUATF_EQ(Quatf(1, 0, 0, 0), Quatf::identity());

  Quatf q(1, 2, 3, 4);
  EXPECT_QUATF_EQ(q, (q * Quatf::identity()));
  EXPECT_QUATF_EQ(q, (Quatf::identity() * q));
  EXPECT_QUATF_EQ(q, (Quatf::identity() * q * Quatf::identity().conjugated()));
}

TEST(QuaternionTest, Substraction)
{


}

TEST(QuaternionTest, Multiplication)
{

}

TEST(QuaternionTest, ScalarMultiplication)
{

}

TEST(QuaternionTest, Division)
{

}

TEST(QuaternionTest, Conversion)
{

}

TEST(QuaternionTest, MatrixRotation)
{
  float pi2 = PI / 2;
  Vec4f x(1, 0, 0, 1);
  Vec4f y(0, 1, 0, 1);
  Vec4f z(0, 0, 1, 1);
  Quatf q_zero_rotation = Quatf::from_axis_angle(Vec3f(0, 0, 1), 0);
  Mat4f a = q_zero_rotation.rotation_matrix();
  Mat4f b = q_zero_rotation.rotation_matrix_slow();
  Vec4f rx = a * x;
  Vec4f ry = a * y;
  Vec4f rz = a * z;
  EXPECT_MAT4F_EQ(a, b);
  EXPECT_VEC4F_EQ(Vec4f(1, 0, 0, 1), rx);
  EXPECT_VEC4F_EQ(Vec4f(0, 1, 0, 1), ry);
  EXPECT_VEC4F_EQ(Vec4f(0, 0, 1, 1), rz);

  Quatf q_x_rotation = Quatf::from_axis_angle(Vec3f(1, 0, 0), pi2);
  a = q_x_rotation.rotation_matrix();
  b = q_x_rotation.rotation_matrix_slow();
  rx = a * x;
  ry = a * y;
  rz = a * z;
  EXPECT_MAT4F_NEAR(a, b, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(1,  0, 0, 1), rx, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(0,  0, 1, 1), ry, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(0, -1, 0, 1), rz, ERROR);

  Quatf q_y_rotation = Quatf::from_axis_angle(Vec3f(0, 1, 0), pi2);
  a = q_y_rotation.rotation_matrix();
  b = q_y_rotation.rotation_matrix_slow();
  rx = a * x;
  ry = a * y;
  rz = a * z;
  EXPECT_MAT4F_NEAR(a, b, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(0, 0, -1, 1), rx, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(0, 1,  0, 1), ry, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(1, 0,  0, 1), rz, ERROR);

  Quatf q_z_rotation = Quatf::from_axis_angle(Vec3f(0, 0, 1), pi2);
  a = q_z_rotation.rotation_matrix();
  b = q_z_rotation.rotation_matrix_slow();
  rx = a * x;
  ry = a * y;
  rz = a * z;
  EXPECT_MAT4F_NEAR(a, b, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f( 0,  1, 0, 1), rx, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f(-1,  0, 0, 1), ry, ERROR);
  EXPECT_VEC4F_NEAR(Vec4f( 0,  0, 1, 1), rz, ERROR);
}

TEST(QuaternionTest, Rotation)
{
  const Vec3f x_axis(1, 0, 0);
  const Vec3f y_axis(0, 1, 0);
  const Vec3f z_axis(0, 0, 1);
  const Quatf qrx = Quatf::from_axis_angle(x_axis, PI2).normalized();
  const Quatf qry = Quatf::from_axis_angle(y_axis, PI2).normalized();
  const Quatf qrz = Quatf::from_axis_angle(z_axis, PI2).normalized();


  EXPECT_VEC3F_NEAR(x_axis, rotate(qrx, x_axis), EPSILON);
  EXPECT_VEC3F_NEAR(y_axis, rotate(qry, y_axis), EPSILON);
  EXPECT_VEC3F_NEAR(z_axis, rotate(qrz, z_axis), EPSILON);

  EXPECT_VEC3F_NEAR(Quatf(x_axis), rotate(qrx, x_axis), EPSILON);
  EXPECT_VEC3F_NEAR(Quatf(y_axis), rotate(qry, y_axis), EPSILON);
  EXPECT_VEC3F_NEAR(Quatf(z_axis), rotate(qrz, z_axis), EPSILON);

  /// @todo test other rotations
}

TEST(QuaternionTest, FromToRotation)
{
  Quatf qxy = Quatf::from_to_rotation(AXIS_X, AXIS_Y);
  ASSERT_QUATF_NEAR(qxy, Quatf::from_axis_angle(AXIS_Z, PI2), EPSILON);

  Quatf qyz = Quatf::from_to_rotation(AXIS_Y, AXIS_Z);
  ASSERT_QUATF_NEAR(qyz, Quatf::from_axis_angle(AXIS_X, PI2), EPSILON);

  Quatf qzx = Quatf::from_to_rotation(AXIS_Z, AXIS_X);
  ASSERT_QUATF_NEAR(qzx, Quatf::from_axis_angle(AXIS_Y, PI2), EPSILON);
}

TEST(QuaternionTest, FromToRotationColinear)
{
  Quatf qxx = Quatf::from_to_rotation(AXIS_X, AXIS_X);
  ASSERT_QUATF_NEAR(qxx, Quatf(), EPSILON);

  Quatf qyy = Quatf::from_to_rotation(AXIS_Y, AXIS_Y);
  ASSERT_QUATF_NEAR(qyy, Quatf(), EPSILON);

  Quatf qzz = Quatf::from_to_rotation(AXIS_Z, AXIS_Z);
  ASSERT_QUATF_NEAR(qzz, Quatf(), EPSILON);
}

TEST(QuaternionTest, FromToRotationColinearInverted)
{
  Quatf qxx = Quatf::from_to_rotation(AXIS_X, -AXIS_X, AXIS_Y);
  ASSERT_QUATF_NEAR(qxx, Quatf::from_axis_angle(AXIS_Y, PI), EPSILON);

  Quatf qyy = Quatf::from_to_rotation(AXIS_Y, -AXIS_Y, AXIS_Z);
  ASSERT_QUATF_NEAR(qyy, Quatf::from_axis_angle(AXIS_Z, PI), EPSILON);

  Quatf qzz = Quatf::from_to_rotation(AXIS_Z, -AXIS_Z, AXIS_X);
  ASSERT_QUATF_NEAR(qzz, Quatf::from_axis_angle(AXIS_X, PI), EPSILON);
}

}
