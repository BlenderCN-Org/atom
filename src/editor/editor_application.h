#pragma once

#include <QApplication>
#include <QUndoStack>
#include <QSettings>
#include <core/ptr.h>
#include <core/corefwd.h>
#include "editorfwd.h"
#include "ui/editor_window.h"

namespace atom {
namespace editor {

class EditorApplication : public QApplication {
  Q_OBJECT
public:
  EditorApplication(int &argc, char *argv[]);
  ~EditorApplication();


  /**
   * Initialize game engine, load game library, create empty world and then display main window.
   * @return true on success, else false (app should quit in this case)
   */
  bool init();

  void process_arguments();

  void notify_entity_changed(sptr<Entity> entity);

  QUndoStack& undo_stack();

  sptr<World> world() const;

  Core& core() const;

  bool is_loaded() const;

  bool notify(QObject *obj, QEvent *event) override;

signals:
  /**
   * Emitted after gamelib has been loaded
   */
  void load();

  /**
   * Emitted when gamelib is noging to be unloaded (on gamelib reload, quit, ...)
   */
  void unload();

  void entity_changed(sptr<Entity> entity);

private:  // pozor na poradie, je dolezite, hlavne my_is_loaded musi byt pred widgetmi
  Core       *my_core;
  bool          my_is_loaded;
  QSettings     my_settings;
  QUndoStack    my_undo_stack;
  sptr<World>   my_world;
  EditorWindow  my_main_window;
};

EditorApplication& application();

QUndoStack& undo_stack();

}
}
