#pragma once

#include <QListWidget>
#include <core/game_entry.h> // NOTE(majo33): kvoli VectorObjectUPtr
#include "editorfwd.h"

namespace atom {
namespace editor {

class EntityList : public QListWidget {
  Q_OBJECT
public:
  EntityList(QWidget *parent = nullptr);

  void load_entity_list();
  void unload_entity_list();

private slots:
  void load();
  void unload();

//  void set_context(const EditorContext *context);

protected:
//  QMimeData* mimeData(const QList<QListWidgetItem *> items) const override;
  QStringList mimeTypes() const override;
  QMimeData* mimeData(const QList<QListWidgetItem *> items) const override;
};

}
}
