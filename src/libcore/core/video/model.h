#pragma once

#include "../foundation.h"
#include "../stdvec.h"

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

struct ElementArray {
  String     name;
  Type       type;
  uptr<u8[]> data;
  u32        size;
};

class Model {
  std::vector<uptr<ElementArray>> my_arrays;

public:
  std::vector<DataBone> bones;

  bool add_array(const String &name, Type type, uptr<u8[]> &&data, u32 size);
  const ElementArray* find_array(const String &name, Type type) const;
};

}
