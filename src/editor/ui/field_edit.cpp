#include "field_edit.h"
#include <core/log.h>
#include "../field_value.h"
#include "../constants.h"
#include "../ui/vec2f_edit.h"

namespace atom {
namespace editor {

FieldEdit::FieldEdit(QWidget *parent)
  : QWidget(parent)
{
  my_layout = new QHBoxLayout(this);
  my_layout->setContentsMargins(0, 0, 0, 0);
}

FieldEdit::~FieldEdit()
{
  log::debug(DEBUG_EDITOR, "Destroying field edit");
}

String FieldEdit::field_name() const
{
  return my_field_name;
}

void FieldEdit::set_field_name(const String &name)
{
  my_field_name = name;
}

void FieldEdit::set_widget(QWidget *widget)
{
  my_layout->addWidget(widget);
}

void FieldEdit::notify_value_changed()
{
  emit value_changed();
}

//
// int
//

FieldEditInt::FieldEditInt(QWidget *parent)
  : FieldEdit(parent)
{
  my_spin_box = new QSpinBox(this);
  set_widget(my_spin_box);
  my_spin_box->setRange(INT_FIELD_MIN, INT_FIELD_MAX);
  connect(my_spin_box, SIGNAL(valueChanged(int)), SLOT(on_value_changed(int)));
}

uptr<FieldValue> FieldEditInt::value() const
{
  return uptr<FieldValue>(new FieldValueInt(my_spin_box->value()));
}

void FieldEditInt::set_value(const FieldValue &value)
{
  if (value.type() == Type::I32) {
    const FieldValueInt &int_value = dynamic_cast<const FieldValueInt &>(value);
    my_spin_box->setValue(int_value.value());
  } else {
    log::error("Invalid value type");
  }
}

void FieldEditInt::on_value_changed(int value)
{
  notify_value_changed();
}

//
// float
//

FieldEditFloat::FieldEditFloat(QWidget *parent)
  : FieldEdit(parent)
{
  my_spin_box = new QDoubleSpinBox(this);
  set_widget(my_spin_box);
  my_spin_box->setRange(FLOAT_FIELD_MIN, FLOAT_FIELD_MAX);
  connect(my_spin_box, SIGNAL(valueChanged(double)), SLOT(on_value_changed(double)));
}

uptr<FieldValue> FieldEditFloat::value() const
{
  return uptr<FieldValue>(new FieldValueFloat(my_spin_box->value()));
}

void FieldEditFloat::set_value(const FieldValue &value)
{
  if (value.type() == Type::F32) {
    const FieldValueFloat &float_value = dynamic_cast<const FieldValueFloat &>(value);
    my_spin_box->setValue(float_value.value());
  } else {
    log::error("Invalid value type");
  }
}

void FieldEditFloat::on_value_changed(double)
{
  notify_value_changed();
}

//
// String
//

FieldEditString::FieldEditString(QWidget *parent)
  : FieldEdit(parent)
{
  my_line_edit = new QLineEdit(this);
  set_widget(my_line_edit);
  connect(my_line_edit, SIGNAL(editingFinished()), SLOT(on_editing_finished()));
}

uptr<FieldValue> FieldEditString::value() const
{
  return uptr<FieldValue>(new FieldValueString(my_line_edit->text().toStdString()));
}

void FieldEditString::set_value(const FieldValue &value)
{
  if (value.type() == Type::STRING) {
    const FieldValueString &string_value = dynamic_cast<const FieldValueString &>(value);
    my_line_edit->setText(QString::fromStdString(string_value.value()));
  } else {
    log::error("Invalid value type");
  }
}

void FieldEditString::on_editing_finished()
{
  notify_value_changed();
}

//
// Vec2f
//

FieldEditVec2f::FieldEditVec2f(QWidget *parent)
{
  my_edit = new Vec2fEdit(this);
  set_widget(my_edit);
  connect(my_edit, SIGNAL(value_changed(Vec2f)), SLOT(on_value_changed(Vec2f)));
}

uptr<FieldValue> FieldEditVec2f::value() const
{
  return uptr<FieldValue>(new FieldValueVec2f(my_edit->value()));
}

void FieldEditVec2f::set_value(const FieldValue &value)
{
  if (value.type() == Type::VEC2F) {
    const FieldValueVec2f &vec2f_value = dynamic_cast<const FieldValueVec2f &>(value);
    my_edit->set_value(vec2f_value.value());
  } else {
    log::error("Invalid value type");
  }
}

void FieldEditVec2f::on_value_changed(Vec2f value)
{
  notify_value_changed();
}

}
}

#include "editor/ui/field_edit.moc"
