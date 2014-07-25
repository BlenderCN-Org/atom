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

class EditorWindow : public QMainWindow {
Q_OBJECT
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

  void load();
  void unload();

  void selection_changed(const std::vector<sptr<Entity>> &selection);

  void clean_changed(bool clean);

  void update_world();

private:
  enum class Mode {
    EDIT,
    GAME
  };

  void switch_mode(Mode mode);

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
  struct DockVisibility {
    bool entity_list;
    bool entity_edit;
    bool undo_view;
  };

private:
  uptr<Ui::MainWindow> my_ui;
  Mode                 my_mode;           ///< current editor mode
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
  bool                 my_grab_input;
  DockVisibility       my_dock_visibility;

#ifdef _WIN32
  i64                  my_last_update;
#endif
};

}
}
