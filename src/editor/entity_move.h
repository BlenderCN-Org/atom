#pragma once

#include <core/foundation.h>
#include "editor_command.h"

namespace atom {
namespace editor {

class EntityMove : public EditorCommand {
public:
  EntityMove(const sptr<Entity> &entity, const Vec3f &start, const Vec3f &end);

  void redo() override;

  void undo() override;

private:
  sptr<Entity> my_entity;
  Vec3f        my_start;
  Vec3f        my_end;
};

}
}
