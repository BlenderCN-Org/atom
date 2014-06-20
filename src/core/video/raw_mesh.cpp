#include "raw_mesh.h"
#include <algorithm>
#include "core/log.h"

namespace atom {

bool RawMesh::add_array(const String &name, Type type, uptr<u8> &&data, u32 size)
{
  uptr<ElementArray> array(new ElementArray());
  array->name = name;
  array->type = type;
  array->data = std::move(data);
  array->size = size;

  my_arrays.push_back(std::move(array));

  log::info("Adding array %s (%i)", name.c_str(), size);
  return true;
}

const ElementArray* RawMesh::find_array(const String &name) const
{
  auto found = std::find_if(my_arrays.begin(), my_arrays.end(),
    [&name](const uptr<ElementArray> &array) { return array->name == name; });

  return found != my_arrays.end() ? found->get() : nullptr;
}


}
