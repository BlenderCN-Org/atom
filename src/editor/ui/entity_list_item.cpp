#include "entity_list_item.h"
#include <cassert>
#include <core/world/entity.h>
#include "../utils.h"

namespace atom {
namespace editor {

EntityListItem::EntityListItem(const QString &name, int index)
  : my_name(name)
  , my_index(index)
{
  assert(!name.isEmpty() && "Object name can't be empty");
  setText(my_name);
  setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

int EntityListItem::get_index() const
{
  return my_index;
}

}
}
