#pragma once

#include "math.h"

namespace atom {

struct Transformations {
  Mat4f projection;
  Mat4f view;
  Mat4f model;

  Transformations() {}

  Transformations(
    const Mat4f &m,
    const Mat4f &v,
    const Mat4f &p)
    : projection(p)
    , view(v)
    , model(m)
  {}

  Mat4f model_view() const
  {
    return view * model;
  }

  Mat4f model_view_projection() const
  {
    return projection * view * model;
  }
};

}
