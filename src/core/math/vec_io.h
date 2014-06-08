#pragma once

#include <iosfwd>
#include <iterator>
#include "core/math/vec_array.h"

namespace atom {

template<typename T>
inline std::ostream& operator<<(
  std::ostream &os,
  const Vec2<T> &v)
{
  return os << v[0] << " " << v[1];
}

template<typename T>
inline std::istream& operator>>(
  std::istream &is,
  Vec2<T> &v)
{
  return is >> v[0] >> v[1];
}

template<typename T>
inline std::istream& operator>>(
  std::istream &is,
  Vec3<T> &v)
{
  return is >> v[0] >> v[1] >> v[2];
}

template<typename T>
inline std::ostream& operator<<(
  std::ostream &os,
  const Vec3<T> &v)
{
  return os << v[0] << " " << v[1] << " " << v[2];
}

template<typename T>
std::istream& operator>>(
  std::istream &is,
  const Vec4<T> &v)
{
  return is >> v[0] >> v[1] >> v[2] >> v[3];
}

template<typename T>
std::ostream& operator<<(
  std::ostream &os,
  const Vec4<T> &v)
{ return os << v[0] << " " << v[1] << " " << v[2] << " " << v[3]; }

/**
 * Zapis obsahu Vec2fArray do vystupneho prudu.
 */
inline std::ostream& operator<<(
  std::ostream &os,
  const Vec2fArray &array)
{
  std::copy(array.begin(), array.end(), std::ostream_iterator<Vec2f>(os, "\n"));
  return os;
}

/**
 * Zapis obsahu Vec3fArray do vystupneho prudu.
 */
inline std::ostream& operator<<(
  std::ostream &os,
  const Vec3fArray &array)
{
  std::copy(array.begin(), array.end(), std::ostream_iterator<Vec3f>(os, "\n"));

  return os;
}

}
