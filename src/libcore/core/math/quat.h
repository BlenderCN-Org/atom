#pragma once

#include "vec.h"
#include "mat.h"
#include "axis_angle.h"

namespace atom {

/**
 * Quaternion template is type independent.
 * The quaternion_utils.h defines float quaternion Quatf, this is used everywhere.
 * Most of the operations requires normalized quaternion (performance reasons).
 */
template<typename T>
struct Quat {
public:
  /**
   * Construct identity quaternion (1, 0, 0, 0)
   */
  static Quat<T> identity()
  { return Quat<T>(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)); }

  /**
   * Create quaternion representing angle/axis rotation.
   *
   * @param x axis
   * @param y axis
   * @param z axis
   * @param angle rotation angle
   *
   * @note axis must be normalized.
   */
  static Quat<T> from_axis_angle(T x, T y, T z, T angle)
  {
    T a = std::sin(angle * static_cast<T>(0.5));
    return Quat<T>(std::cos(angle * static_cast<T>(0.5)), x * a, y * a, z * a);
  }

  /**
   * Create quaternion representing angle/axis rotation.
   *
   * @param v axis (must be unit vector)
   * @param angle
   */
  static Quat<T> from_axis_angle(const Vec3<T> &v, T angle)
  {
    return from_axis_angle(v.x, v.y, v.z, angle);
  }

  /**
   * Create quaternion from axis angle.
   *
   * @param v axis/angle representation (axis must be unit vector)
   */
  static Quat<T> from_axis_angle(const AxisAngle<T> &v)
  {
    return from_axis_angle(v.axis, v.angle);
  }
  
  static Quat<T> from_to_rotation(const Vec3<T> &from, const Vec3<T> &to)
  {
    const Vec3<T> v = cross_product(from, to);
    const T w = sqrt((from.length2()) * (to.length2())) + dot_product(from, to);
    return Quat<T>(w, v).normalized();
  }

  Quat()
    : x(0)
    , y(0)
    , z(0)
    , w(1)
  {
  }

  /**
   * Construct quaternion from w, x, y, z coefficients.
   */
  Quat(T qw, T qx, T qy, T qz)
    : x(qx)
    , y(qy)
    , z(qz)
    , w(qw)
  {}

  /**
   * Construct quaternion from w, x, y, z coefficients.
   *
   * @param qw w coefficient
   * @param qv direction coefficients
   */
  Quat(T qw, const Vec3<T> &qv)
    : w(qw)
    , x(qv.x)
    , y(qv.y)
    , z(qv.z)
  {}

  Quat(const Vec3<T> &v)
   : x(v.x)
   , y(v.y)
   , z(v.z)
   , w(1)
  {}

// not used, neviem preco tu je tento konstruktor
//  explicit Quaternion(const Vec3<T> &v)
//    : w(static_cast<T>(0))
//    , x(v.x)
//    , y(v.y)
//    , z(v.z)
//  {}

  /**
   * Default copy constructor (quaternion doesn't need special one).
   */
  Quat(const Quat<T> &q)
    : x(q.x)
    , y(q.y)
    , z(q.z)
    , w(q.w)
  {}

  /**
   * Default copy operator (quaternion doesn't need special one).
   */
  Quat<T>& operator=(const Quat<T> &q) = default;

  /**
   * Preved kvaternion na rotacnu maticu (predpoklada sa ze kvaternion je normalizovany).
   * Using right handed coordinate system.
   */
  Mat4<T> rotation_matrix() const
  {
    T xx2 = 2 * x * x;
    T yy2 = 2 * y * y;
    T zz2 = 2 * z * z;
    T xy2 = 2 * x * y;
    T xz2 = 2 * x * z;
    T wx2 = 2 * x * w;
    T yz2 = 2 * y * z;
    T wy2 = 2 * y * w;
    T wz2 = 2 * z * w;

    return Mat4<T>(
      1 - yy2 - zz2,     xy2 - wz2,     xz2 + wy2, 0,
          xy2 + wz2, 1 - xx2 - zz2,     yz2 - wx2, 0,
          xz2 - wy2,     yz2 + wx2, 1 - xx2 - yy2, 0,
                  0,             0,             0, 1);
  }

  /**
   * Preved kvaternion na rotacnu maticu (prevedie aj nenormalizovany kvaternion).
   * Using right handed coordinate system.
   */
  Mat4<T> rotation_matrix_slow() const
  {
    T ww  = w * w;
    T xx  = x * x;
    T yy  = y * y;
    T zz  = z * z;
    T xy2 = 2 * x * y;
    T xz2 = 2 * x * z;
    T wx2 = 2 * x * w;
    T yz2 = 2 * y * z;
    T wy2 = 2 * y * w;
    T wz2 = 2 * z * w;

    return Mat4<T>(
      ww + xx - yy -zz,         xy2 - wz2,         xz2 + wy2,                 0,
             xy2 + wz2, ww - xx + yy - zz,         yz2 - wx2,                 0,
             xz2 - wy2,         yz2 + wx2, ww - xx - yy + zz,                 0,
                     0,                 0,                 0, ww + xx + yy + zz);
  }

  /**
   * Konjugovany kvaternion (ma opacny smer).
   */
  Quat<T> conjugated() const
  { return Quat<T>(w, -x, -y, -z); }

  /**
   * Inverzny kvaternion.
   */
  Quat<T> inverted() const
  { return conjugated() / length2(); }

  /**
   * Normalizovany kvaternion (dlzka == 1).
   */
  Quat<T> normalized() const
  { return *this / length(); }

  /**
   * Dlzka/norma kvaternionu.
   */
  T length() const
  { return sqrt(length2()); }

  /**
   * Neodmocnena dlzka/norma kvaternionu (vhodna pre niektore vypocty).
   */
  T length2() const
  {
    return dot_product(*this, *this);
  }

  /**
   * Convert quaternion to axis angle representation.
   * @note quaternion must be normalized.
   *
   * @param[out] axis
   * @param[out] angle
   */
  void to_axis_angle(Vec3<T> &axis, T &angle) const
  {
    angle = 2 * std::acos(w);
    T  sa = sqrt(1.0 - w * w);

    if (sa < 0.0005) {
      axis.x = x;
      axis.y = y;
      axis.z = z;
    } else {
      axis.x = x / sa;
      axis.y = y / sa;
      axis.z = z / sa;
    }
  }

  /**
   * Convert quaternion to axis angle structure.
   *
   * @note quaternion must be normalized
   */
  AxisAngle<T> to_axis_angle() const
  {
    AxisAngle<T> axis_angle;
    to_axis_angle(axis_angle.axis, axis_angle.angle);
    return axis_angle;
  }

public:
  T x; ///< smerova zlozka x
  T y; ///< smerova zlozka y
  T z; ///< smerova zlozka z
  T w; ///< skalarna zlozka
};

/**
 * Dot product dvoch kvaternionov (neodmocnena dlzka).
 */
template<typename T>
T dot_product(const Quat<T> &a, const Quat<T> &b)
{
  return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
bool operator==(const Quat<T> &a, const Quat<T> &b)
{
  return (a.w == b.w) && (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

template<typename T>
bool operator!=(const Quat<T> &a, const Quat<T> &b)
{
  return !operator==(a, b);
}

/**
 * Nasobenie skalarom.
 */
template<typename T>
Quat<T> operator+(const Quat<T> &q, T value)
{
  return Quat<T>(q.w * value, q.x * value, q.y * value, q.z * value);
}

/**
 * Nasobenie skalarom.
 */
template<typename T>
Quat<T> operator+(T value, const Quat<T> &q)
{
  return Quat<T>(q.w * value, q.x * value, q.y * value, q.z * value);
}

/**
 * Delenie skalarom.
 */
template<typename T>
Quat<T> operator/(const Quat<T> &q, T value)
{
  return Quat<T>(q.w / value, q.x / value, q.y / value, q.z / value);
}

/**
 * Sucet dvoch kvaternionov.
 */
template<typename T>
Quat<T> operator+(const Quat<T> &a, const Quat<T> &b)
{
  return Quat<T>(a.w + b.w, a.x + b.x, a.y + b.y, a.z + b.z);
}

/**
 * Sucet dvoch kvaternionov.
 */
template<typename T>
Quat<T> operator+=(Quat<T> &a, const Quat<T> &b)
{
  a.w += b.w;
  a.x += b.x;
  a.y += b.y;
  a.z += b.z;
  return a;
}

/**
 * Rozdiel dvoch kvaternionov.
 */
template<typename T>
Quat<T> operator-(const Quat<T> &a, const Quat<T> &b)
{
  return Quat<T>(a.w - b.w, a.x - b.x, a.y - b.y, a.z - b.z);
}

template<typename T>
Quat<T> operator-=(Quat<T> &a, const Quat<T> &b)
{
  a.w -= b.w;
  a.x -= b.x;
  a.y -= b.y;
  a.z -= b.z;
  return a;
}

/**
 * Nasobenie dvoch kvaternionov.
 */
template<typename T>
Quat<T> operator*(const Quat<T> &a, const Quat<T> &b)
{
  return Quat<T>(
    a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
    a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
    a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
    a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w);
}

/**
 * Delenie dvoch kvaternionov.
 */
template<typename T>
Quat<T> operator/(const Quat<T> &a, const Quat<T> &b)
{
  T l = b.length2();
  return Quat<T>(
    (a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z) / l,
    (a.x * b.w - a.w * b.x - a.z * b.y + a.y * b.z) / l,
    (a.y * b.w + a.z * b.x - a.w * b.y - a.x * b.z) / l,
    (a.z * b.w - a.y * b.x + a.x * b.y - a.w * b.z) / l);
}

//
// Additional quaternion operations
//

/**
 * Rotate 3D point using quaternion rotation.
 *
 * @param[in] q rotation
 * @param[in] v point in 3d
 * @return rotated point around q
 *
 * @note quaternion must be normalized
 */
template<typename T>
Vec3<T> rotate(const Quat<T> &q, const Vec3<T> &v)
{
  Quat<T> rotated = q * Quat<T>(0, v.x, v.y, v.z) * q.conjugated();
  return Vec3<T>(rotated.x, rotated.y, rotated.z);
}

/**
 * Rotate quaternion b around quaternion a.
 *
 * @param[in] a rotation quaternion
 * @param[in] b rotated quaternion
 * @return rotated quaternion
 *
 * \note quaternions must be normalized
 */
template<typename T>
Quat<T> rotate(const Quat<T> &a, const Quat<T> &b)
{ return a * b * a.conjugated(); }

template<typename T>
Quat<T> slerp(const Quat<T> &a, const Quat<T> &b, T t)
{
  // quaternion to return
  Quat<T> q;
  // Calculate angle between them.
  T half_cos = a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
  // if a=b or a=-b then theta = 0 and we can return a
  if (abs(half_cos) >= 1) {
    return a;
  }
  // Calculate temporary values.
  T half_theta = std::acos(half_cos);
  T half_sin = std::sqrt(1 - half_cos * half_cos);
  // if theta = 180 degrees then result is not fully defined
  // we could rotate around any axis normal to a or b
  if (abs(half_sin) < static_cast<T>(0.001)) {
    return Quat<T>(
      a.w * 0.5 + b.w * 0.5,
      a.x * 0.5 + b.x * 0.5,
      a.y * 0.5 + b.y * 0.5,
      a.z * 0.5 + b.z * 0.5);
  }

  T ta = std::sin((1 - t) * half_theta) / half_sin;
  T tb  = std::sin(t * half_theta) / half_sin;

  return Quat<T>(
    a.w * ta + b.w * tb,
    a.x * ta + b.x * tb,
    a.y * ta + b.y * tb,
    a.z * ta + b.z * tb);
}

}
