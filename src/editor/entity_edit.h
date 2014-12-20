#pragma once

#include <vector>
#include <QWidget>
#include <QFormLayout>
#include <core/corefwd.h>
#include <core/string.h>
#include <core/noncopyable.h>
#include "editorfwd.h"

namespace atom {
namespace editor {

class FieldEdit;

class EntityEdit : public QWidget {
  Q_OBJECT
public:
  EntityEdit(QWidget *parent = nullptr);
  ~EntityEdit();

  void set_entity(const sptr<Entity> &entity);

private slots:
  void load();
  void unload();

  void on_field_value_change();

  void on_entity_changed(sptr<Entity> entity);

private:
  void clear_field_edits();
  void reload_field_edits();

  void update_field_values();

  FieldEdit* get_field_edit_by_name(const String &field_name);

  void add_field_to_layout(const QString &label, uptr<FieldEdit> field_edit);

  struct NamedFieldEdit : private NonCopyable {
    String name;
    uptr<FieldEdit> edit;

    NamedFieldEdit(const String &field_name, uptr<FieldEdit> &&field_edit);
    ~NamedFieldEdit();

    NamedFieldEdit(NamedFieldEdit &&src);
  };

private:
  bool                         my_ignore_changes;
  QFormLayout                 *my_layout;
  sptr<Entity>                 my_entity;
  std::vector<NamedFieldEdit>  my_field_edits;  ///< pri zmazani prvkov to zmaze aj widgety
};

}
}
