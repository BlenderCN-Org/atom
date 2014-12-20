#include "entity_add.h"
#include <cassert>
#include <core/world.h>

namespace atom {
namespace editor {

EntityAdd::EntityAdd(const sptr<Entity> &entity, const sptr<World> &world)
  : EditorCommand("entity added")
  , my_entity(entity)
  , my_world(world)
{
  assert(my_entity != nullptr);
  assert(my_world != nullptr);
}

void EntityAdd::redo()
{
  my_world->add_entity(my_entity);
}

void EntityAdd::undo()
{
  my_world->remove_entity(my_entity);
}

}
}
