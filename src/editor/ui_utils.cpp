#include "ui_utils.h"

namespace atom {
namespace editor {


QPoint get_widget_center(const QWidget &widget)
{
  return QPoint(widget.width() / 2, widget.height() / 2);
}

void center_cursor(QWidget &widget)
{
  QCursor::setPos(widget.mapToGlobal(get_widget_center(widget)));
}

}
}
