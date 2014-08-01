#pragma once

#include <cassert>
#include <cmath>

namespace atom {

template<typename T>
struct Vec3 {
  enum {
    SIZE = 3
  };

  typedef T value_type;

  Vec3() {}

  Vec3(T vx, T vy, T vz)
    : x(vx)
    , y(vy)
    , z(vz)
  {}

  static Vec3<T> axis_x()
  { return Vec3<T>(1, 0, 0); }

  static Vec3<T> axis_y()
  { return Vec3<T>(0, 1, 0); }

  static Vec3<T> axis_z()
  { return Vec3<T>(0, 0, 1); }

  T& operator[](unsigned i)
  {
    assert(i < SIZE);
    return data[i];
  }

  const T& operator[](unsigned i) const
  {
    assert(i < SIZE);
    return data[i];
  }

  Vec3<T>& operator+=(const Vec3<T> &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  T length() const
  {
    return std::sqrt(x * x + y * y + z * z);
  }

  T length2() const
  {
    return x * x + y * y + z * z;
  }

  Vec3<T> normalized() const
  {
    T len = length();
    return Vec3(x / len, y / len, z / len);
  }

  union {
    struct {
      T x;
      T y;
      T z;
    };
    T data[SIZE];
  };
};

template<typename T>
Vec3<T> operator-(const Vec3<T> &a)
{
  return Vec3<T>(-a.x, -a.y, -a.z);
}

template<typename T>
Vec3<T> operator+(const Vec3<T> &a, const Vec3<T> &b)
{
  return Vec3<T>(a.x + b.x, a.y + b.y, a.z + b.z);
}

template<typename T>
Vec3<T> operator-(const Vec3<T> &a, const Vec3<T> &b)
{
  return Vec3<T>(a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename T>
Vec3<T> operator*(const Vec3<T> &a, const Vec3<T> &b)
{
  return Vec3<T>(a.x * b.x, a.y * b.y, a.z * b.z);
}

template<typename T>
Vec3<T> operator*(const Vec3<T> &v, T x)
{
  return Vec3<T>(v.x * x, v.y * x, v.z * x);
}

template<typename T>
Vec3<T> operator*(T x, const Vec3<T> &v)
{
  return Vec3<T>(v.x * x, v.y * x, v.z * x);
}

template<typename T>
Vec3<T> operator/(const Vec3<T> &v, T x)
{
  return Vec3<T>(v.x / x, v.y / x, v.z / x);
}

template<typename T>
inline bool operator==(const Vec3<T> &a, const Vec3<T> &b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

template<typename T>
inline bool operator!=(const Vec3<T> &a, const Vec3<T> &b)
{
  return !(a == b);
}

/**
 * Dot product operation for Vec3
 */
template<typename T>
inline T dot_product(
  const Vec3<T> &a,
  const Vec3<T> &b)
{
  return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

template<typename T>
inline T dot_product(
  T ax, T ay, T az,
  T bx, T by, T bz)
{
  return ax * bx + ay * by + az * bz;
}

/**
 * Skalarny sucin.
 */
template<typename T>
inline Vec3<T> cross_product(
  const Vec3<T> &a,
  const Vec3<T> &b)
{
  return Vec3<T>(
    a[1] * b[2] - a[2] * b[1],
    a[2] * b[0] - a[0] * b[2],
    a[0] * b[1] - a[1] * b[0]
  );
}

}
