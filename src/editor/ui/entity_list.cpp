#include "entity_list.h"
#include <QMimeData>
#include "editor/log.h"
#include "editor/ui/entity_list_item.h"
#include "editor/editor_application.h"

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

  for (const EntityCreator &info : application().core().entity_creators()) {
    log::debug(DEBUG_EDITOR, "Adding object to list %s", info.name.c_str());
    addItem(new EntityListItem(QString::fromStdString(info.name), index++));
  }
}

void EntityList::unload_entity_list()
{
  clear();
}

void EntityList::load()
{
  log::debug(DEBUG_EDITOR, "Loading entity list");
  load_entity_list();
}

void EntityList::unload()
{
  log::debug(DEBUG_EDITOR, "Unloading entity list");
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
    log::warning("Only single selected item supported for drag&drop");
    return nullptr;
  }

  const EntityListItem *item = static_cast<const EntityListItem *>(items.front());
  QMimeData *mime_data = new QMimeData(); //QQQ kto je majitel???
  mime_data->setData("mm/object", QString::number(item->get_index()).toLatin1());

  return mime_data;
}

}
}

#include "editor/ui/entity_list.moc"
