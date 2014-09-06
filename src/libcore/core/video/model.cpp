#include "model.h"
#include <algorithm>
#include "../log.h"

namespace atom {

bool Model::add_array(const String &name, Type type, uptr<u8[]> &&data, u32 size)
{
  uptr<ElementArray> array(new ElementArray());
  array->name = name;
  array->type = type;
  array->data = std::move(data);
  array->size = size;

  my_arrays.push_back(std::move(array));
  return true;
}

const ElementArray* Model::find_array(const String &name, Type type) const
{
  auto found = std::find_if(my_arrays.begin(), my_arrays.end(),
    [&name, &type](const uptr<ElementArray> &array)
    {
      return array->name == name && array->type == type;
    });

  return found != my_arrays.end() ? found->get() : nullptr;
}


}