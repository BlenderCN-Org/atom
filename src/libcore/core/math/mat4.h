#pragma once

#include <cassert>
#include <cmath>
#include "../types.h"
#include "mat3.h"
#include "vec.h"

namespace atom {

template<typename T>
struct Mat4 {
  enum {
    SIZE = 4
  };

  typedef T ValueType;

  Vec4<T> data[SIZE];

  static Mat4<T> rotation_x(T angle)
  {
    Mat4<T> m = Mat4<T>::identity();
    T s = std::sin(angle);
    T c = std::cos(angle);

    m.value(1, 1) = c;
    m.value(1, 2) = -s;
    m.value(2, 1) = s;
    m.value(2, 2) = c;
    return m;
  }

  static Mat4<T> rotation_y(T angle)
  {
    Mat4<T> m = Mat4<T>::identity();
    T s = std::sin(angle);
    T c = std::cos(angle);

    m.value(0, 0) = c;
    m.value(0, 2) = s;
    m.value(2, 0) = -s;
    m.value(2, 2) = c;
    return m;
  }

  static Mat4<T> rotation_z(T angle)
  {
    Mat4<T> m = Mat4<T>::identity();
    T s = std::sin(angle);
    T c = std::cos(angle);

    m.value(0, 0) = c;
    m.value(0, 1) = -s;
    m.value(1, 0) = s;
    m.value(1, 1) = c;
    return m;
  }

  static Mat4<T> scale(T sx, T sy, T sz)
  {
    Mat4<T> m = Mat4<T>::identity();
    m.value(0, 0) = sx;
    m.value(1, 1) = sy;
    m.value(2, 2) = sz;
    return m;
  }

  static Mat4<T> orthographic(
    T xmin,
    T xmax,
    T ymin,
    T ymax,
    T zmin,
    T zmax)
  {
    Mat4<T> m = Mat4<T>::identity();
    m.value(0, 0) =  2.0 / (xmax - xmin);
    m.value(1, 1) =  2.0 / (ymax - ymin);
    m.value(2, 2) = -2.0 / (zmax - zmin);

    m.value(0, 3) = -((xmax + xmin) / (xmax - xmin));
    m.value(1, 3) = -((ymax + ymin) / (ymax - ymin));
    m.value(2, 3) = -((zmax + zmin) / (zmax - zmin));
    m.value(3, 3) = 1.0;
    return m;
  }

  static Mat4<T> identity()
  {
    Mat4 result;
    result.value(0, 0) = 1.0;
    result.value(0, 1) = 0.0;
    result.value(0, 2) = 0.0;
    result.value(0, 3) = 0.0;

    result.value(1, 0) = 0.0;
    result.value(1, 1) = 1.0;
    result.value(1, 2) = 0.0;
    result.value(1, 3) = 0.0;

    result.value(2, 0) = 0.0;
    result.value(2, 1) = 0.0;
    result.value(2, 2) = 1.0;
    result.value(2, 3) = 0.0;

    result.value(3, 0) = 0.0;
    result.value(3, 1) = 0.0;
    result.value(3, 2) = 0.0;
    result.value(3, 3) = 1.0;
    return result;
  }

  static Mat4<T> perspective(
    T fov,
    T aspect,
    T near,
    T far)
  {
//    assert(near > 0.0f);
//    assert(near < far);
//    assert(fov > 0.0f);
//    assert(aspect > 0.0f);

    Mat4<T> m = Mat4<T>::identity();
    T ymax = near * tan(fov * 0.5);
    T ymin = -ymax;
    T xmax = ymax * aspect;
    T xmin = -xmax;
    T dx = xmax - xmin;
    T dy = ymax - ymin;
    T dz = far - near;
    m.value(0, 0) = (2.0 * near) / dx;
    m.value(1, 1) = (2.0 * near) / dy;
    m.value(2, 2) = -(far + near) / dz;
    m.value(2, 3) = -(2.0 * near * far) / dz;
    m.value(3, 2) = -1.0;
    m.value(3, 3) = 0.0;
    return m;
  }

  static Mat4<T> translation(T x, T y, T z)
  {
    Mat4<T> m = Mat4<T>::identity();
    m.value(0, 3) = x;
    m.value(1, 3) = y;
    m.value(2, 3) = z;
    return m;
  }

  static Mat4<T> translation(const Vec3<T> &v)
  {
    return translation(v.x, v.y, v.z);
  }

  static Mat4<T> infinite_perspective(
    T fov,
    T aspect,
    T near)
  {
    assert(fov > 0.0f);
    assert(aspect > 0.0f);
    assert(near > 0.0f);

    Mat4<T> m = Mat4<T>::identity();
    T ymax = near * tan(fov * 0.5);
    T ymin = -ymax;
    T xmax = ymax * aspect;
    T xmin = -xmax;
    T dx = xmax - xmin;
    T dy = ymax - ymin;
    m.value(0, 0) = (2.0 * near) / dx;
    m.value(1, 1) = (2.0 * near) / dy;
    m.value(2, 2) = -1;
    m.value(2, 3) = -(2.0 * near);
    m.value(3, 2) = -1.0;
    m.value(3, 3) = 0.0;

    return m;
  }

  T& value(unsigned row, unsigned column)
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  const T& value(unsigned row, unsigned column) const
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  T& operator()(unsigned row, unsigned column)
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  const T& operator()(unsigned row, unsigned column) const
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  Vec4<T>& operator[](unsigned column)
  {
    assert(column < SIZE);
    return data[column];
  }

  const Vec4<T>& operator[](unsigned column) const
  {
    assert(column < SIZE);
    return data[column];
  }

  Mat4() = default;

  Mat4(
    T v00, T v01, T v02, T v03,
    T v10, T v11, T v12, T v13,
    T v20, T v21, T v22, T v23,
    T v30, T v31, T v32, T v33)
  {
    value(0, 0) = v00;  value(0, 1) = v01;  value(0, 2) = v02;  value(0, 3) = v03;
    value(1, 0) = v10;  value(1, 1) = v11;  value(1, 2) = v12;  value(1, 3) = v13;
    value(2, 0) = v20;  value(2, 1) = v21;  value(2, 2) = v22;  value(2, 3) = v23;
    value(3, 0) = v30;  value(3, 1) = v31;  value(3, 2) = v32;  value(3, 3) = v33;
  }

  void rotate_x(T angle)
  {
    *this = *this * Mat4::rotation_x(angle);
  }

  void rotate_y(T angle)
  {
    *this = *this * Mat4::rotation_y(angle);
  }

  void rotate_z(T angle)
  {
    *this = *this * Mat4::rotation_z(angle);
  }

  Mat3<T> to_3x3() const
  {
    return Mat3<T>(
      value(0, 0), value(0, 1), value(0, 2),
      value(1, 0), value(1, 1), value(1, 2),
      value(2, 0), value(2, 1), value(2, 2));
  }

  Mat4<T> inverted() const
  {
    T a = value(0, 0);
    T b = value(0, 1);
    T c = value(0, 2);
    T d = value(0, 3);
    T e = value(1, 0);
    T f = value(1, 1);
    T g = value(1, 2);
    T h = value(1, 3);
    T i = value(2, 0);
    T j = value(2, 1);
    T k = value(2, 2);
    T l = value(2, 3);
    T m = value(3, 0);
    T n = value(3, 1);
    T o = value(3, 2);
    T p = value(3, 3);

    // 2x2 determinants
    T abef = a * f - b * e;
    T bcfg = b * g - c * f;
    T cdgh = c * h - d * g;
    T aceg = a * g - c * e;
    T bdfh = b * h - d * f;
    T adeh = a * h - d * e;

    T ijmn = i * n - j * m;
    T jkno = j * o - k * n;
    T klop = k * p - l * o;
    T ikmo = i * o - k * m;
    T jlnp = j * p - l * n;
    T ilmp = i * p - l * m;

    // 3x3 determinants
    T aa = f * klop - g * jlnp + h * jkno;
    T bb = e * klop - g * ilmp + h * ikmo;
    T cc = e * jlnp - f * ilmp + h * ijmn;
    T dd = e * jkno - f * ikmo + g * ijmn;
    T ee = b * klop - c * jlnp + d * jkno;
    T ff = a * klop - c * ilmp + d * ikmo;
    T gg = a * jlnp - b * ilmp + d * ijmn;
    T hh = a * jkno - b * ikmo + c * ijmn;

    T ii = n * cdgh - o * bdfh + p * bcfg;
    T jj = m * cdgh - o * adeh + p * aceg;
    T kk = m * bdfh - n * adeh + p * abef;
    T ll = m * bcfg - n * aceg + o * abef;
    T mm = j * cdgh - k * bdfh + l * bcfg;
    T nn = i * cdgh - k * adeh + l * aceg;
    T oo = i * bdfh - j * adeh + l * abef;
    T pp = i * bcfg - j * aceg + k * abef;

    // determinant of 4x4 matrix, expanded by cofactors
    T det = a * aa - b * bb + c * cc - d * dd;
    T mul = 1.0 / det;

    Mat4<T> mat;
    mat.value(0, 0) =  aa * mul;
    mat.value(0, 1) = -ee * mul;
    mat.value(0, 2) =  ii * mul;
    mat.value(0, 3) = -mm * mul;
    mat.value(1, 0) = -bb * mul;
    mat.value(1, 1) =  ff * mul;
    mat.value(1, 2) = -jj * mul;
    mat.value(1, 3) =  nn * mul;
    mat.value(2, 0) =  cc * mul;
    mat.value(2, 1) = -gg * mul;
    mat.value(2, 2) =  kk * mul;
    mat.value(2, 3) = -oo * mul;
    mat.value(3, 0) = -dd * mul;
    mat.value(3, 1) =  hh * mul;
    mat.value(3, 2) = -ll * mul;
    mat.value(3, 3) =  pp * mul;
    return mat;
  }
};

/**
 * Add operation.
 */
template<typename T>
Mat4<T> operator+(const Mat4<T> &a, const Mat4<T> &b)
{
  Mat4<T> result;

  for (unsigned i = 0; i < 4; ++i) {
    result[i] = a[i] + b[i];
  }

  return result;
}

/**
 * Multiply two matrices.
 */
template<typename T>
Mat4<T> operator*(const Mat4<T> &a, const Mat4<T> &b)
{
  Mat4<T> result;

  for (unsigned i = 0; i < 4; ++i) {
    for (unsigned j = 0; j < 4; ++j) {
      result(i, j) = a(i, 0) * b(0, j)
                   + a(i, 1) * b(1, j)
                   + a(i, 2) * b(2, j)
                   + a(i, 3) * b(3, j);
    }
  }

  return result;
}

template<typename T>
Vec4<T> operator*(const Mat4<T> &m, const Vec4<T> &v)
{
  T x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2] + m(0, 3) * v[3];
  T y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2] + m(1, 3) * v[3];
  T z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2] + m(2, 3) * v[3];
  T w = m(3, 0) * v[0] + m(3, 1) * v[1] + m(3, 2) * v[2] + m(3, 3) * v[3];
  return Vec4<T>(x, y, z, w);
}

template<typename T>
Vec3<T> operator*(const Mat4<T> &m, const Vec3<T> &v)
{
  T x = m(0, 0) * v[0] + m(0, 1) * v[1] + m(0, 2) * v[2];
  T y = m(1, 0) * v[0] + m(1, 1) * v[1] + m(1, 2) * v[2];
  T z = m(2, 0) * v[0] + m(2, 1) * v[1] + m(2, 2) * v[2];
  T w = m(3, 0) * v[0] + m(3, 1) * v[1] + m(3, 2) * v[2] + m(3, 3);
  return Vec3<T>(x / w, y / w, z / w);
}

}
