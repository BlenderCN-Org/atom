#pragma once

#include <core/corefwd.h>
#include <core/math/vec.h>
#include <core/ptr.h>
#include "editor_command.h"

namespace atom {
namespace editor {

class EntityMove : public EditorCommand {
public:
  EntityMove(const sptr<Entity> &entity, const Vec2f &start, const Vec2f &end);

  void redo() override;

  void undo() override;

private:
  sptr<Entity> my_entity;
  Vec2f        my_start;
  Vec2f        my_end;
};

}
}
