#include "entity_edit.h"
#include "../log.h"
#include "../editor_application.h"
#include "../utils.h"
#include "../ui/field_edit.h"
#include "../commands/change_entity_field.h"

namespace atom {
namespace editor {
namespace {

uptr<FieldEdit> create_field_edit(Type type)
{
  uptr<FieldEdit> field_edit;

  switch (type) {
    case Type::I32:
      field_edit.reset(new FieldEditInt());
      break;

    case Type::F32:
      field_edit.reset(new FieldEditFloat());
      break;

    case Type::STRING:
      field_edit.reset(new FieldEditString());
      break;

    case Type::VEC2F:
      field_edit.reset(new FieldEditVec2f());
      break;

    default:
      log::warning("Entity edit is not able to edit this field");
      break;
  }

  return field_edit;
}

}

EntityEdit::EntityEdit(QWidget *parent)
  : QWidget(parent)
  , my_ignore_changes(false)
  , my_layout(nullptr)
{
  EditorApplication &app = application();
  connect(&app, SIGNAL(entity_changed(sptr<Entity>)),
    SLOT(on_entity_changed(sptr<Entity>)));
}

EntityEdit::~EntityEdit()
{
  clear_field_edits();
}

void EntityEdit::set_entity(const sptr<Entity> &entity)
{
  if (my_entity == entity) {
    return;
  }

  log::debug(DEBUG_EDITOR, "Setting current entity to %p", entity.get());
  my_entity = entity;

  clear_field_edits();

  if (my_entity != nullptr) {
    reload_field_edits();
  }
}

void EntityEdit::load()
{
  log::debug(DEBUG_EDITOR, "Loading entity edit");
}

void EntityEdit::unload()
{
  log::debug(DEBUG_EDITOR, "Unloading entity edit");
  set_entity(nullptr);
}

void EntityEdit::on_field_value_change()
{
  if (my_ignore_changes) {
    return;
  }

  FieldEdit *field_edit = qobject_cast<FieldEdit*>(sender());

  if (field_edit == nullptr) {
    log::error("Invalid sender");
    return;
  }

  EditorCommand *command = new ChangeEntityField(my_entity, field_edit->field_name(),
    field_edit->value());
  undo_stack().push(command);
}

void EntityEdit::on_entity_changed(sptr<Entity> entity)
{
  if (my_entity == entity) {
    update_field_values();
  }
}

void EntityEdit::clear_field_edits()
{
  log::debug(DEBUG_EDITOR, "Removing all edit fields");

  if (my_layout != nullptr) {
    for (const NamedFieldEdit &field_edit : my_field_edits) {
      log::debug(DEBUG_EDITOR, "Removing field edit \"%s\"", field_edit.name.c_str());
      delete my_layout->labelForField(field_edit.edit.get());
    }

    delete my_layout;
    my_field_edits.clear();
    my_layout = nullptr;
  }
}

void EntityEdit::reload_field_edits()
{
  assert(my_layout == nullptr);
  assert(layout() == nullptr);

  if (my_entity == nullptr) {
    log::debug(DEBUG_EDITOR, "No entity selected, no fields to reload");
    return;
  }

  const MetaClass *meta_class = my_entity->meta;

  if (meta_class == nullptr) {
    log::warning("Entity without meta class, each entity should has metaclass");
    return;
  }

  my_layout = new QFormLayout();
  setLayout(my_layout);

  // enumerate field in the entity and create corresponding field edit widgets
  MetaFieldsEnumarator enumerator(*meta_class);
  const MetaField *field;

  while ((field = enumerator.next())) {
    uptr<FieldEdit> field_edit = create_field_edit(field->type);

    if (field_edit != nullptr) {
      field_edit->set_field_name(field->name);
      add_field_to_layout(field->name, std::move(field_edit));
    } else {
      log::warning("This field type is not supported by EntityEdit");
    }
  }

  update_field_values();
}

void EntityEdit::update_field_values()
{
  if (my_entity == nullptr) {
    return;
  }

  const MetaClass *meta_class = my_entity->meta;

  if (meta_class == nullptr) {
    log::warning("Entity without meta class, each entity should has metaclass");
    return;
  }

  my_ignore_changes = true;

  MetaFieldsEnumarator enumerator(*meta_class);
  const MetaField *field;

  while ((field = enumerator.next())) {
    uptr<FieldValue> value = create_field_value_for_type(field->type);

    if (value == nullptr) {
      log::warning("This type of field (%i) is not supported, skipping", field->type);
      continue;
    }

    value->load_value_from(ptr_with_offset(my_entity.get(), field->offset));
    FieldEdit *edit = get_field_edit_by_name(field->name);

    if (edit != nullptr) {
      edit->set_value(*value);
    } else {
      log::warning("Can't find field edit for property \"%s\"", field->name);
    }
  }

  my_ignore_changes = false;
}

FieldEdit *EntityEdit::get_field_edit_by_name(const String &field_name)
{
  auto found = std::find_if(my_field_edits.begin(), my_field_edits.end(),
    [&field_name](const NamedFieldEdit &edit)
    { return edit.name == field_name; });

  return found != my_field_edits.end() ? found->edit.get() : nullptr;
}

void EntityEdit::add_field_to_layout(const QString &label, uptr<FieldEdit> field_edit)
{
  assert(field_edit != nullptr);
  log::debug(DEBUG_EDITOR, "Adding field edit for \"%s\"", label.toStdString().c_str());
  my_layout->addRow(label, field_edit.get());

  connect(field_edit.get(), SIGNAL(value_changed()), SLOT(on_field_value_change()));

  my_field_edits.emplace_back(label.toStdString(), std::move(field_edit));
}

EntityEdit::NamedFieldEdit::NamedFieldEdit(const String &field_name, uptr<FieldEdit> &&field_edit)
  : name(field_name)
  , edit(std::move(field_edit))
{
}

EntityEdit::NamedFieldEdit::~NamedFieldEdit()
{
}

EntityEdit::NamedFieldEdit::NamedFieldEdit(EntityEdit::NamedFieldEdit &&src)
  : name(std::move(src.name))
  , edit(std::move(src.edit))
{
}

}
}

#include "editor/ui/entity_edit.moc"
