#pragma once

#include <QListWidgetItem>
#include <core/corefwd.h>

namespace atom {
namespace editor {

class EntityListItem : public QListWidgetItem {
public:
  EntityListItem(const QString &name, int index);

  int get_index() const;

private:
  QString my_name;
  int     my_index;    ///< object index/position in the object list
};

}
}
