#include <gtest/gtest.h>
#include <core/math/math.h>
#include <core/utils/utils.h>

namespace atom {

constexpr float ERROR = 0.001f;

#define EXPECT_QUATF_EQ(expected, q) \
  EXPECT_FLOAT_EQ(expected.w, q.w);  \
  EXPECT_FLOAT_EQ(expected.x, q.x);  \
  EXPECT_FLOAT_EQ(expected.y, q.y);  \
  EXPECT_FLOAT_EQ(expected.z, q.z);

#define EXPECT_MAT4F_EQ(expected, m)       \
  EXPECT_FLOAT_EQ(expected(0, 0), m(0, 0)); \
  EXPECT_FLOAT_EQ(expected(0, 1), m(0, 1)); \
  EXPECT_FLOAT_EQ(expected(0, 2), m(0, 2)); \
  EXPECT_FLOAT_EQ(expected(0, 3), m(0, 3)); \
  EXPECT_FLOAT_EQ(expected(1, 0), m(1, 0)); \
  EXPECT_FLOAT_EQ(expected(1, 1), m(1, 1)); \
  EXPECT_FLOAT_EQ(expected(1, 2), m(1, 2)); \
  EXPECT_FLOAT_EQ(expected(1, 3), m(1, 3)); \
  EXPECT_FLOAT_EQ(expected(2, 0), m(2, 0)); \
  EXPECT_FLOAT_EQ(expected(2, 1), m(2, 1)); \
  EXPECT_FLOAT_EQ(expected(2, 2), m(2, 2)); \
  EXPECT_FLOAT_EQ(expected(2, 3), m(2, 3)); \
  EXPECT_FLOAT_EQ(expected(3, 0), m(3, 0)); \
  EXPECT_FLOAT_EQ(expected(3, 1), m(3, 1)); \
  EXPECT_FLOAT_EQ(expected(3, 2), m(3, 2)); \
  EXPECT_FLOAT_EQ(expected(3, 3), m(3, 3));

#define EXPECT_MAT4F_NEAR(expected, m, abs_error)       \
  EXPECT_NEAR(expected(0, 0), m(0, 0), abs_error); \
  EXPECT_NEAR(expected(0, 1), m(0, 1), abs_error); \
  EXPECT_NEAR(expected(0, 2), m(0, 2), abs_error); \
  EXPECT_NEAR(expected(0, 3), m(0, 3), abs_error); \
  EXPECT_NEAR(expected(1, 0), m(1, 0), abs_error); \
  EXPECT_NEAR(expected(1, 1), m(1, 1), abs_error); \
  EXPECT_NEAR(expected(1, 2), m(1, 2), abs_error); \
  EXPECT_NEAR(expected(1, 3), m(1, 3), abs_error); \
  EXPECT_NEAR(expected(2, 0), m(2, 0), abs_error); \
  EXPECT_NEAR(expected(2, 1), m(2, 1), abs_error); \
  EXPECT_NEAR(expected(2, 2), m(2, 2), abs_error); \
  EXPECT_NEAR(expected(2, 3), m(2, 3), abs_error); \
  EXPECT_NEAR(expected(3, 0), m(3, 0), abs_error); \
  EXPECT_NEAR(expected(3, 1), m(3, 1), abs_error); \
  EXPECT_NEAR(expected(3, 2), m(3, 2), abs_error); \
  EXPECT_NEAR(expected(3, 3), m(3, 3), abs_error);

#define EXPECT_VEC4F_EQ(expected, v) \
  EXPECT_FLOAT_EQ(expected.x, v.x);   \
  EXPECT_FLOAT_EQ(expected.y, v.y);   \
  EXPECT_FLOAT_EQ(expected.z, v.z);   \
  EXPECT_FLOAT_EQ(expected.w, v.w);

#define EXPECT_VEC4F_NEAR(expected, v, abs_error) \
  EXPECT_NEAR(expected.x, v.x, abs_error);        \
  EXPECT_NEAR(expected.y, v.y, abs_error);        \
  EXPECT_NEAR(expected.z, v.z, abs_error);        \
  EXPECT_NEAR(expected.w, v.w, abs_error);

#define EXPECT_VEC3F_NEAR(expected, v) \
  EXPECT_FLOAT_EQ(expected.x, v.x);        \
  EXPECT_FLOAT_EQ(expected.y, v.y);        \
  EXPECT_FLOAT_EQ(expected.z, v.z);

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

TEST(QuaternionTest, QuaternionRotation)
{
  const Vec3f x_axis(1, 0, 0);
  const Vec3f y_axis(0, 1, 0);
  const Vec3f z_axis(0, 0, 1);
  const Quatf qrx = Quatf::from_axis_angle(x_axis, PI / 2).normalized();
  const Quatf qry = Quatf::from_axis_angle(y_axis, PI / 2).normalized();
  const Quatf qrz = Quatf::from_axis_angle(z_axis, PI / 2).normalized();


  EXPECT_VEC3F_NEAR(x_axis, rotate(qrx, x_axis));
  EXPECT_VEC3F_NEAR(y_axis, rotate(qry, y_axis));
  EXPECT_VEC3F_NEAR(z_axis, rotate(qrz, z_axis));

  EXPECT_VEC3F_NEAR(Quatf(x_axis), rotate(qrx, x_axis));
  EXPECT_VEC3F_NEAR(Quatf(y_axis), rotate(qry, y_axis));
  EXPECT_VEC3F_NEAR(Quatf(z_axis), rotate(qrz, z_axis));

  /// @todo test other rotations
}

}
