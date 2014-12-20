#pragma once

#include "foundation.h"
#include "stdvec.h"

namespace atom {

struct DataBone {
  String name;
  i32    parent;
  Vec3f  head;
  Vec3f  tail;
  Vec3f  local_head;
  Vec3f  local_tail;
  Vec3f  x; // x axis
  Vec3f  y; // y axis (toward bone)
  Vec3f  z; // z axis
};

struct DataStream {
  String          name;
  Type            type;
  std::vector<u8> data;
};

class Model {
  std::vector<uptr<DataStream>> my_arrays;

public:
  std::vector<DataBone> bones;

  bool add_array(const String &name, Type type, std::vector<u8> &&data);
  const DataStream* find_array(const String &name, Type type) const;

  template<typename T>
  Slice<T> find_stream(const String &name) const
  {
    const DataStream *stream = find_array(name, type_of<T>());

    if (stream != nullptr && stream->type == type_of<T>()) {
      return Slice<T>(reinterpret_cast<const T *>(&stream->data[0]),
        stream->data.size() / sizeof(T));
    }
    // return empty slice when stream not found or invalid data type
    return Slice<T>();
  }
};

}
