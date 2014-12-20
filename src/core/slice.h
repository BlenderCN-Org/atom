#pragma once

#include "platform.h"

namespace atom {

/**
 * Slice represents a segment of an array (view into array). It is a very
 * lightweight object. Can be used to iterate over section of array.
 * Slice doesn't take ownership of any object in the array.
 * Slice instance is valid as long as the array exists.
 */
template<typename T>
class Slice {
  const T *my_data;   ///< pointer to the first element
  u32      my_count;  ///< total element count

public:
  /**
   * Construct empty slice.
   */
  Slice()
    : my_data(nullptr)
    , my_count(0)
  {}

  /**
   * Construct slice that starts at @p data and contains @p count elements.
   *
   * @param data pointer to the first element
   * @param count number of elements in this slice
   */
  Slice(const T *data, u32 count)
    : my_data(data)
    , my_count(count)
  {
  }

  bool is_empty() const
  {
    return my_data == nullptr;
  }

  const T* begin() const
  {
    return my_data;
  }

  const T* end() const
  {
    return my_data + my_count;
  }

  const T* data() const
  {
    return my_data;
  }

  u32 size() const
  {
    return my_count;
  }
  
  u32 raw_size() const
  {
    return my_count * sizeof(T);
  }

  T& operator[](u32 index)
  {
    return my_data[index];
  }

  const T& operator[](u32 index) const
  {
    return my_data[index];
  }
};

}
