#include "vec2f_edit.h"
#include <QLabel>
#include <QHBoxLayout>
#include "constants.h"

namespace atom {
namespace editor {

Vec2fEdit::Vec2fEdit(QWidget *parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  layout->addWidget(new QLabel("x: "));
  my_x_spin_box = new QDoubleSpinBox();
  layout->addWidget(my_x_spin_box, 1);
  my_x_spin_box->setRange(FLOAT_FIELD_MIN, FLOAT_FIELD_MAX);
  connect(my_x_spin_box, SIGNAL(valueChanged(double)), SLOT(on_spin_box_value_changed(double)));
  layout->addWidget(new QLabel("y: "));
  my_y_spin_box = new QDoubleSpinBox();
  layout->addWidget(my_y_spin_box, 1);
  my_y_spin_box->setRange(FLOAT_FIELD_MIN, FLOAT_FIELD_MAX);
  connect(my_y_spin_box, SIGNAL(valueChanged(double)), SLOT(on_spin_box_value_changed(double)));
}

Vec2f Vec2fEdit::value() const
{
  return my_current_value;
}

void Vec2fEdit::set_value(const Vec2f &v)
{
  my_x_spin_box->setValue(v.x);
  my_y_spin_box->setValue(v.y);
}

void Vec2fEdit::on_spin_box_value_changed(double)
{
  Vec2f value(my_x_spin_box->value(), my_y_spin_box->value());

  if (value != my_current_value) {
    my_current_value = value;
    emit value_changed(my_current_value);
  }
}

}
}

#include "editor/vec2f_edit.moc"
