#pragma once

#include "core/math/vec2.h"

namespace atom {

template<typename T>
struct Mat2 {
  enum {
    SIZE = 2
  };

  Vec2<T> data[SIZE];

  Mat2() {};

  Mat2(
    T v00, T v01,
    T v10, T v11)
  {
    value(0, 0) = v00;
    value(0, 1) = v01;
    value(1, 0) = v10;
    value(1, 1) = v11;
  }

  T& value(unsigned row, unsigned column)
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  T value(unsigned row, unsigned column) const
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

  T operator()(unsigned row, unsigned column) const
  {
    assert(row < SIZE);
    assert(column < SIZE);
    return data[column][row];
  }

  Vec2<T>& operator[](unsigned column)
  {
    assert(column < SIZE);
    return data[column];
  }

  const Vec2<T>& operator[](unsigned column) const
  {
    assert(column < SIZE);
    return data[column];
  }
};

}
