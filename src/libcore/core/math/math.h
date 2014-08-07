#pragma once

#include "vec.h"
#include "mat.h"
#include "quat.h"

namespace atom {

/**
 * Vypocitaj koeficienty roviny (a, b, c, d) z troch vrcholov.
 *
 * Vypocitane koeficienty su ulozene v stvorprvkovom vektore v poradi a, b, c, d.
 * Z koeficientov a, b, c sa da ziskat normalovy vektor roviny (Vec3(a, b, c)).
 */
Vec4f plane_from_vertices(const Vec3f &v0, const Vec3f &v1, const Vec3f &v2);

template<typename T>
T bezier(const T &p0, const T &p1, const T &p2, const T &p3, typename T::value_type t)
{
  typedef typename T::value_type V;
  V u = 1 - t;
  V u2 = u * u;
  V u3 = u * u2;
  V t2 = t * t;
  V t3 = t * t2;

  if (t < 0)
    return p0;
  else if (t > 1)
    return p3;
  else
    return (u3 * p0) + (3 * u2 * t * p1) + (3 * u * t2 * p2) + (t3 * p3);
}

template<typename T>
T clamp(T x, T min, T max)
{
  return x < min ? min : (x > max ? max : x);
}

template<typename T>
T lerp(T x, T min, T max)
{
  return min + x * (max - min);
}

}
