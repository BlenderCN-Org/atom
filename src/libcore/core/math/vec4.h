#pragma once

namespace atom {

template<typename T>
struct Vec4 {
  enum {
    SIZE = 4
  };

  typedef T value_type;

  Vec4() = default;

  Vec4(T vx, T vy, T vz, T vw)
    : x(vx)
    , y(vy)
    , z(vz)
    , w(vw)
  {}

  Vec4(const Vec3<T> &v, T w)
  {
    data[0] = v[0];
    data[1] = v[1];
    data[2] = v[2];
    data[3] = w;
  }

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

  union {
    struct {
      T x;
      T y;
      T z;
      T w;
    };
    T data[SIZE];
  };
};

template<typename T>
Vec4<T> operator+(const Vec4<T> &a, const Vec4<T> &b)
{
  return Vec4<T>(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

template<typename T>
Vec4<T> operator-(const Vec4<T> &a, const Vec4<T> &b)
{
  return Vec4<T>(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

template<typename T>
Vec4<T> operator/(const Vec4<T> &v, T a)
{
  return Vec4<T>(v.x / a, v.y / a, v.z / a, v.w / a);
}

template<typename T>
Vec4<T> operator/=(Vec4<T> &v, T a)
{
  v.x /= a;
  v.y /= a;
  v.z /= a;
  v.w /= a;
  return v;
}

template<typename T>
inline bool operator==(const Vec4<T> &a, const Vec4<T> &b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template<typename T>
inline bool operator!=(const Vec4<T> &a, const Vec4<T> &b)
{
  return !(a == b);
}

}