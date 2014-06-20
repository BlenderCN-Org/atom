#pragma once

#include "core/corefwd.h"
#include "core/stdvec.h"
#include "core/video/stream_type.h"
#include "core/noncopyable.h"

namespace atom {

struct ElementArray {
  String   name;
  Type     type;
  uptr<u8> data;
  u32      size;
};

class RawMesh {
private:
  std::vector<uptr<ElementArray>> my_arrays;
  ElementArray *my_vertices;
  ElementArray *my_normals;
  ElementArray *my_indices;

public:
  bool add_array(const String &name, Type type, uptr<u8> &&data, u32 size);
  const ElementArray* find_array(const String &name) const;
};

}
