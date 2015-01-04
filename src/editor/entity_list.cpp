#include "entity_list.h"
#include <QMimeData>
#include "log.h"
#include "entity_list_item.h"
#include "editor_application.h"

namespace atom {
namespace editor {

EntityList::EntityList(QWidget *parent)
  : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);
  setDragEnabled(true);

  connect(qApp, SIGNAL(load()), SLOT(load()));
  connect(qApp, SIGNAL(unload()), SLOT(unload()));

  if (application().is_loaded()) {
    load();
  }
}

void EntityList::load_entity_list()
{
  clear();

  int index = 0;

  for (const EntityDefinition &info : application().core().entity_creators()) {
    log_debug(DEBUG_EDITOR, "Adding object to list %s", info.name);
    addItem(new EntityListItem(QString::fromStdString(info.name), index++));
  }
}

void EntityList::unload_entity_list()
{
  clear();
}

void EntityList::load()
{
  log_debug(DEBUG_EDITOR, "Loading entity list");
  load_entity_list();
}

void EntityList::unload()
{
  log_debug(DEBUG_EDITOR, "Unloading entity list");
  unload_entity_list();
}

QStringList EntityList::mimeTypes() const
{
  return QStringList("mm/object");
}

QMimeData* EntityList::mimeData(const QList<QListWidgetItem *> items) const
{
  assert(items.size() == 1 && "Only single selection mode is supported");

  if (items.size() != 1) {
    log_warning("Only single selected item supported for drag&drop");
    return nullptr;
  }

  const EntityListItem *item = static_cast<const EntityListItem *>(items.front());
  QMimeData *mime_data = new QMimeData();
  mime_data->setData("mm/object", QString::number(item->get_index()).toLatin1());

  return mime_data;
}

}
}

#include "editor/entity_list.moc"
