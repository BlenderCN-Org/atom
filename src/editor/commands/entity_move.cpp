#include "entity_move.h"
#include <core/world/entity.h>
#include "editor/editor_application.h"

namespace atom {
namespace editor {

EntityMove::EntityMove(const sptr<Entity> &entity, const Vec3f &start,
  const Vec3f &end)
  : EditorCommand("entity move")
  , my_entity(entity)
  , my_start(start)
  , my_end(end)
{
  assert(my_entity != nullptr);
}

void EntityMove::redo()
{
//  my_entity->set_position(my_end);
  /// @todo reinicializovat entitu
  application().notify_entity_changed(my_entity);
}

void EntityMove::undo()
{
//  my_entity->set_position(my_start);
  /// @todo reinicializovat entitu
  application().notify_entity_changed(my_entity);
}

}
}
