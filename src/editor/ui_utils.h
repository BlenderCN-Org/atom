#pragma once

#include <QWidget>

namespace atom {
namespace editor {

QPoint get_widget_center(const QWidget &widget);

void center_cursor(QWidget &widget);

}
}
