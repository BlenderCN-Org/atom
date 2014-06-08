#pragma once

#include <cassert>
#include "core/types.h"

namespace atom {

template<typename T>
struct Vec2 {
  enum {
    SIZE = 2
  };

  typedef T value_type;

  Vec2() {}

  Vec2(T x, T y)
  {
    data[0] = x;
    data[1] = y;
  }

  T& operator[](unsigned i)
  {
    assert(i < SIZE);
    return data[i];
  }

  T operator[](unsigned i) const
  {
    assert(i < SIZE);
    return data[i];
  }

  bool is_zero() const
  {
    return x == 0 && y == 0;
  }

  Vec2<T>& operator+=(const Vec2<T> &v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  union {
    struct {
      T x;
      T y;
    };
    T data[SIZE];
  };
};

template<typename T>
inline const Vec2<T> operator+(const Vec2<T> &a, const Vec2<T> &b)
{
  return Vec2<T>(a.x + b.x, a.y + b.y);
}

template<typename T>
inline const Vec2<T> operator-(const Vec2<T> &a, const Vec2<T> &b)
{
  return Vec2<T>(a.x - b.x, a.y - b.y);
}

template<typename T>
inline const Vec2<T> operator*(const Vec2<T> &a, T b)
{
  return Vec2<T>(a.x * b, a.y * b);
}

template<typename T>
inline const Vec2<T> operator*(T a, const Vec2<T> &b)
{
  return Vec2<T>(a * b.x, a * b.y);
}

template<typename T>
inline bool operator==(const Vec2<T> &a, const Vec2<T> &b)
{
  return a.x == b.x && a.y == b.y;
}

template<typename T>
inline bool operator!=(const Vec2<T> &a, const Vec2<T> &b)
{
  return !(a == b);
}

/**
 * Dot product operation for Vec2
 */
template<typename T>
inline T dot_product(
  const Vec2<T> &a,
  const Vec2<T> &b)
{ return a[0] * b[0] + a[1] * b[1]; }

template<typename T>
Vec2<T> normalized(const Vec2<T> &v)
{
  if (v[0] > EPSILON && v[1] > EPSILON) {
    T w = sqrt(v[0] * v[0] + v[1] * v[1]);
    return Vec2<T>(v[0] / w, v[1] / w);
  } else {
    return v;
  }
}

}
