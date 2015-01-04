#include "change_entity_field.h"
#include <cassert>
#include <core/meta.h>
#include <core/world.h>
#include "field_value.h"
#include "utils.h"
#include "editor_application.h"

namespace atom {
namespace editor {

ChangeEntityField::ChangeEntityField(const sptr<Entity> &entity, const String &field_name,
  uptr<FieldValue> &&new_value)
  : EditorCommand("change value")
  , my_entity(entity)
  , my_field_name(field_name)
  , my_new_value(std::move(new_value))
{
  assert(my_entity != nullptr);
  assert(my_new_value != nullptr);
  assert(!my_field_name.empty());
}

ChangeEntityField::~ChangeEntityField()
{
}

void ChangeEntityField::redo()
{
  const MetaField &field = meta_field();

  my_prev_value = create_field_value_for_type(field.type);

  if (my_prev_value == nullptr) {
    log_error("This type %i is not supported by field value", field.type);
    return;
  }

  my_prev_value->load_value_from(ptr_with_offset(my_entity.get(), field.offset));
  my_new_value->set_value_to(ptr_with_offset(my_entity.get(), field.offset));
  ///  @todo reinicializovat entitu
  application().notify_entity_changed(my_entity);
}

void ChangeEntityField::undo()
{
  my_prev_value->set_value_to(ptr_with_offset(my_entity.get(), meta_field().offset));
  /// @todo reinicializovat entitu
  application().notify_entity_changed(my_entity);
}

const MetaField& ChangeEntityField::meta_field() const
{
  const MetaField *field = my_entity->meta->find_field(my_field_name.c_str());

  if (field == nullptr) {
    log_error("The entity doesn't have field \"%s\"", my_field_name.c_str());
  }

  return *field;
}

}
}
