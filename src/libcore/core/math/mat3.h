#pragma once

#include "../types.h"
#include "vec3.h"

namespace atom {

template<typename T>
struct Mat3 {
  enum {
    SIZE = 3
  };

  Vec3<T> data[SIZE];

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

  Vec3<T>& operator[](unsigned column)
  {
    assert(column < SIZE);
    return data[column];
  }

  const Vec3<T>& operator[](unsigned column) const
  {
    assert(column < SIZE);
    return data[column];
  }

  Mat3() = default;

  Mat3(
    T v00, T v01, T v02,
    T v10, T v11, T v12,
    T v20, T v21, T v22)
  {
    value(0, 0) = v00;  value(0, 1) = v01;  value(0, 2) = v02;
    value(1, 0) = v10;  value(1, 1) = v11;  value(1, 2) = v12;
    value(2, 0) = v20;  value(2, 1) = v21;  value(2, 2) = v22;
  }
};

}
