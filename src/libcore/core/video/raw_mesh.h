#pragma once

#include "../foundation.h"
#include "../stdvec.h"

namespace atom {

struct Bone {
  String name;
  i32    parent;
  Vec3f  head;
  Vec3f  tail;
  Vec3f  local_head;
  Vec3f  local_tail;
};

struct ElementArray {
  String     name;
  Type       type;
  uptr<u8[]> data;
  u32        size;
};

class RawMesh {
private:
  std::vector<uptr<ElementArray>> my_arrays;

public:
  std::vector<Bone> bones;

public:
  bool add_array(const String &name, Type type, uptr<u8[]> &&data, u32 size);
  const ElementArray* find_array(const String &name) const;
};

}
