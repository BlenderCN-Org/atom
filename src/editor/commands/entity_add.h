#pragma once

#include <core/corefwd.h>
#include <core/ptr.h>
#include "editor_command.h"

namespace atom {
namespace editor {

class EntityAdd : public EditorCommand {
public:
  EntityAdd(const sptr<Entity> &entity, const sptr<World> &world);

  void redo() override;

  void undo() override;

private:
  sptr<Entity> my_entity;
  sptr<World>  my_world;
};

}
}
