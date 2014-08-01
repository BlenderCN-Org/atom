#pragma once

#include "platform.h"

namespace atom {

template<typename T>
class Slice {
public:
  Slice()
    : my_data(nullptr)
    , my_count(0)
  {
  }

  Slice(const T *data, u32 count)
    : my_data(data)
    , my_count(count)
  {
  }

  bool is_empty() const
  {
    return my_data == nullptr;
  }

private:
  const T *my_data;
  u32      my_count;
};

}
