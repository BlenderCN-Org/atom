#pragma once

#include <core/foundation.h>

namespace atom {

const float ERROR = 0.001f;

#define EXPECT_QUATF_EQ(expected, q)                                  \
  EXPECT_FLOAT_EQ(expected.w, q.w);                                   \
  EXPECT_FLOAT_EQ(expected.x, q.x);                                   \
  EXPECT_FLOAT_EQ(expected.y, q.y);                                   \
  EXPECT_FLOAT_EQ(expected.z, q.z);

#define EXPECT_MAT4F_EQ(expected, m)                                  \
  EXPECT_FLOAT_EQ(expected(0, 0), m(0, 0));                           \
  EXPECT_FLOAT_EQ(expected(0, 1), m(0, 1));                           \
  EXPECT_FLOAT_EQ(expected(0, 2), m(0, 2));                           \
  EXPECT_FLOAT_EQ(expected(0, 3), m(0, 3));                           \
  EXPECT_FLOAT_EQ(expected(1, 0), m(1, 0));                           \
  EXPECT_FLOAT_EQ(expected(1, 1), m(1, 1));                           \
  EXPECT_FLOAT_EQ(expected(1, 2), m(1, 2));                           \
  EXPECT_FLOAT_EQ(expected(1, 3), m(1, 3));                           \
  EXPECT_FLOAT_EQ(expected(2, 0), m(2, 0));                           \
  EXPECT_FLOAT_EQ(expected(2, 1), m(2, 1));                           \
  EXPECT_FLOAT_EQ(expected(2, 2), m(2, 2));                           \
  EXPECT_FLOAT_EQ(expected(2, 3), m(2, 3));                           \
  EXPECT_FLOAT_EQ(expected(3, 0), m(3, 0));                           \
  EXPECT_FLOAT_EQ(expected(3, 1), m(3, 1));                           \
  EXPECT_FLOAT_EQ(expected(3, 2), m(3, 2));                           \
  EXPECT_FLOAT_EQ(expected(3, 3), m(3, 3));

#define EXPECT_MAT4F_NEAR(expected, m, abs_error)                     \
  EXPECT_NEAR(expected(0, 0), m(0, 0), abs_error);                    \
  EXPECT_NEAR(expected(0, 1), m(0, 1), abs_error);                    \
  EXPECT_NEAR(expected(0, 2), m(0, 2), abs_error);                    \
  EXPECT_NEAR(expected(0, 3), m(0, 3), abs_error);                    \
  EXPECT_NEAR(expected(1, 0), m(1, 0), abs_error);                    \
  EXPECT_NEAR(expected(1, 1), m(1, 1), abs_error);                    \
  EXPECT_NEAR(expected(1, 2), m(1, 2), abs_error);                    \
  EXPECT_NEAR(expected(1, 3), m(1, 3), abs_error);                    \
  EXPECT_NEAR(expected(2, 0), m(2, 0), abs_error);                    \
  EXPECT_NEAR(expected(2, 1), m(2, 1), abs_error);                    \
  EXPECT_NEAR(expected(2, 2), m(2, 2), abs_error);                    \
  EXPECT_NEAR(expected(2, 3), m(2, 3), abs_error);                    \
  EXPECT_NEAR(expected(3, 0), m(3, 0), abs_error);                    \
  EXPECT_NEAR(expected(3, 1), m(3, 1), abs_error);                    \
  EXPECT_NEAR(expected(3, 2), m(3, 2), abs_error);                    \
  EXPECT_NEAR(expected(3, 3), m(3, 3), abs_error);

#define EXPECT_VEC4F_EQ(expected, v)                                  \
  EXPECT_FLOAT_EQ(expected.x, v.x);                                   \
  EXPECT_FLOAT_EQ(expected.y, v.y);                                   \
  EXPECT_FLOAT_EQ(expected.z, v.z);                                   \
  EXPECT_FLOAT_EQ(expected.w, v.w);

#define EXPECT_VEC4F_NEAR(expected, v, abs_error)                     \
  EXPECT_NEAR(expected.x, v.x, abs_error);                            \
  EXPECT_NEAR(expected.y, v.y, abs_error);                            \
  EXPECT_NEAR(expected.z, v.z, abs_error);                            \
  EXPECT_NEAR(expected.w, v.w, abs_error);

#define EXPECT_VEC3F_EQ(expected, v)                                  \
  EXPECT_FLOAT_EQ(expected.x, v.x);                                   \
  EXPECT_FLOAT_EQ(expected.y, v.y);                                   \
  EXPECT_FLOAT_EQ(expected.z, v.z);   

#define EXPECT_VEC3F_NEAR(expected, v, abs_error)                     \
  EXPECT_NEAR(expected.x, v.x, abs_error);                            \
  EXPECT_NEAR(expected.y, v.y, abs_error);                            \
  EXPECT_NEAR(expected.z, v.z, abs_error);

}
