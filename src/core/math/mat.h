#pragma once

#include "core/math/mat2.h"
#include "core/math/mat3.h"
#include "core/math/mat4.h"

namespace atom {

typedef Mat2<float> Mat2f;
typedef Mat3<float> Mat3f;
typedef Mat4<float> Mat4f;

/**
 * Multiply Mat4 (using only submatrix 3x3) and Vec3.
 * It is equivalent of m * Vec4(v, 0).
 */
template<typename T>
Vec3<T> rotate(const Mat4<T> &m, const Vec3<T> &v)
{
  return Vec3<T>(
    m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z,
    m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z,
    m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z);
}

}
