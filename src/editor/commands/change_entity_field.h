#pragma once

#include <core/corefwd.h>
#include <core/meta.h>
#include <core/string.h>
#include "editor_command.h"
#include "editor/editorfwd.h"

namespace atom {
namespace editor {

class ChangeEntityField : public EditorCommand {
public:
  ChangeEntityField(const sptr<Entity> &entity, const String &field_name,
    uptr<FieldValue> &&new_value);

  ~ChangeEntityField();

  void redo() override;

  void undo() override;

private:
  const MetaField& meta_field() const;

private:
  sptr<Entity> my_entity;
  String              my_field_name;
  uptr<FieldValue>    my_prev_value;
  uptr<FieldValue>    my_new_value;
};

}
}
