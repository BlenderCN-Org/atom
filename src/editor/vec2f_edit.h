#pragma once

#include <QWidget>
#include <QDoubleSpinBox>
#include <core/math.h>

namespace atom {
namespace editor {

class Vec2fEdit : public QWidget {
  Q_OBJECT
public:
  Vec2fEdit(QWidget *parent = nullptr);

  Vec2f value() const;

  void set_value(const Vec2f &v);

signals:
  void value_changed(Vec2f value);

private slots:
  void on_spin_box_value_changed(double);

private:
  Vec2f           my_current_value;
  QDoubleSpinBox *my_x_spin_box;
  QDoubleSpinBox *my_y_spin_box;
};

}
}
