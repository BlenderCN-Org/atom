#include "model.h"
#include <algorithm>
#include "../log.h"

namespace atom {

bool Model::add_array(const String &name, Type type, std::vector<u8> &&data)
{
  uptr<DataStream> array(new DataStream());
  array->name = name;
  array->type = type;
  array->data = std::move(data);

  my_arrays.push_back(std::move(array));
  return true;
}

const DataStream* Model::find_array(const String &name, Type type) const
{
  auto found = std::find_if(my_arrays.begin(), my_arrays.end(),
    [&name, &type](const uptr<DataStream> &array)
    {
      return array->name == name && array->type == type;
    });

  return found != my_arrays.end() ? found->get() : nullptr;
}


}
