#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <core/foundation.h>
#include "../editorfwd.h"
#include "../field_value.h"

namespace atom {
namespace editor {

class FieldEdit : public QWidget {
  Q_OBJECT
public:
  FieldEdit(QWidget *parent = nullptr);
  ~FieldEdit();

  String field_name() const;

  void set_field_name(const String &name);

  virtual uptr<FieldValue> value() const = 0;

  virtual void set_value(const FieldValue &value) = 0;

signals:
  void value_changed();

protected:
  void set_widget(QWidget *widget);

  void notify_value_changed();

private:
  QHBoxLayout *my_layout;
  String       my_field_name;
};

class FieldEditInt : public FieldEdit {
  Q_OBJECT
public:
  FieldEditInt(QWidget *parent = nullptr);

  uptr<FieldValue> value() const override;

  void set_value(const FieldValue &value) override;

private slots:
  void on_value_changed(int value);

private:
  QSpinBox *my_spin_box;
};

class FieldEditFloat : public FieldEdit {
  Q_OBJECT
public:
  FieldEditFloat(QWidget *parent = nullptr);

  virtual uptr<FieldValue> value() const override;

  void set_value(const FieldValue &value) override;

private slots:
  void on_value_changed(double);

private:
  QDoubleSpinBox *my_spin_box;
};

class FieldEditString : public FieldEdit {
  Q_OBJECT
public:
  FieldEditString(QWidget *parent = nullptr);

  virtual uptr<FieldValue> value() const override;

  void set_value(const FieldValue &value) override;

private slots:
  void on_editing_finished();

private:
  QLineEdit *my_line_edit;
};

class FieldEditVec2f : public FieldEdit {
  Q_OBJECT
public:
  FieldEditVec2f(QWidget *parent = nullptr);

  virtual uptr<FieldValue> value() const override;

  void set_value(const FieldValue &value) override;

private slots:
  void on_value_changed(Vec2f value);

private:
  Vec2fEdit *my_edit;
};

}
}
