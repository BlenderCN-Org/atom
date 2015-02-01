#include "editor_application.h"

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <core/core.h>
#include <core/game_entry.h>
#include <core/world.h>
#include <core/exception.h>
#include "log.h"

namespace atom {
namespace editor {

EditorApplication::EditorApplication(int &argc, char *argv[])
  : QApplication(argc, argv)
  , my_core(nullptr)
  , my_is_loaded(false)
{
}

EditorApplication::~EditorApplication()
{
  emit unload();

  my_undo_stack.clear();
  my_world = nullptr;
  Core::quit();
}

bool EditorApplication::init_engine()
{
  Core &core = Core::init(InitMode::EDITOR, game_entry());
  my_core = &core;

  my_world = World::create(core);
  my_is_loaded = true;
  emit load();
  my_main_window.show();
  return true;
}

void EditorApplication::process_arguments()
{
  QStringList args = arguments();

  // load saved level
  if (args.size() == 2) {
    my_main_window.load_file(QString(LEVEL_RESOURCE_DIR) + "/" + args[1] + "." + LEVEL_FILE_EXT);
  }
}

void EditorApplication::notify_entity_changed(sptr<Entity> entity)
{
  emit entity_changed(entity);
}

QUndoStack& EditorApplication::undo_stack()
{
  return my_undo_stack;
}

sptr<World> EditorApplication::world() const
{
  return my_world;
}

Core& EditorApplication::core() const
{
  assert(my_core != nullptr && "You've forget to initialize Core");
  return *my_core;
}

bool EditorApplication::is_loaded() const
{
  return my_is_loaded;
}

EditorApplication& application()
{
  return *static_cast<EditorApplication *>(qApp);
}

QUndoStack &undo_stack()
{
  return application().undo_stack();
}

}
}

#include "editor/editor_application.moc"
