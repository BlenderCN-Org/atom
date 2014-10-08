#pragma once

#include <QMainWindow>
#include <QUndoStack>
#include <QUndoView>
#include <QTimer>
#include <memory>
#include <core/system/core.h>
#include <core/system/game_api.h>
#include "../editorfwd.h"

namespace atom {
namespace editor {

class EntityList;
class GameView;

enum class EditorWindowMode {
  EDIT,
  GAME
};

struct EditorWindowPanels {
  bool entity_list;
  bool entity_edit;
  bool undo_view;
};

class EditorWindow : public QMainWindow {
Q_OBJECT
  
  uptr<Ui::MainWindow> my_ui;
  EditorWindowMode     my_mode;             ///< current editor mode
  i64                  my_update_interval;
  GameView            *my_game_view;
  EntityList          *my_entity_list;
  EntityEdit          *my_entity_edit;
  QString              my_filename;
  QTimer               my_refresh_timer;
  sptr<World>          my_clone;
  QUndoView           *my_undo_view;
  QAction             *my_undo_action;
  QAction             *my_redo_action;
  EditorWindowPanels   my_panels;           ///< panel visibility state
  
#ifdef _WIN32
  i64                  my_last_update;
#endif
  
public:
  EditorWindow();
  ~EditorWindow();

  void clear();

  bool eventFilter(QObject *watched, QEvent *event);

  void load_file(const QString &filename);
  
protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void on_action_open_triggered();
  void on_action_save_triggered();
  void on_action_save_as_triggered();
  void on_action_quit_triggered();
  void on_action_run_triggered();

  void on_action_show_physics_triggered(bool checked);
  void on_action_show_bounding_box_triggered(bool checked);
  void on_action_show_aabb_triggered(bool checked);

  void load();
  void unload();

  void selection_changed(const std::vector<sptr<Entity>> &selection);

  void clean_changed(bool clean);

  void update_world();

private:
  

  void switch_mode(EditorWindowMode mode);

  void init_menu_view();
  void init_game_view();
  void init_undo_view();

  void add_undo_redo();
  void remove_undo_redo();

  void restore_geometry();
  void save_geometry();

  QString make_window_title(bool clean) const;

  QString document_name() const;

  bool eventFilterKey(QKeyEvent &event);
  bool eventFilterMouseKey(QMouseEvent &event);
  bool eventFilterMouseMove(QMouseEvent &event);

private:

};

}
}
