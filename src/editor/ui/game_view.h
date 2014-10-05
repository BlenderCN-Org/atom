#pragma once

#include <QGLWidget>
#include <QTimer>
#include <core/corefwd.h>
#include "../editorfwd.h"

namespace atom {
namespace editor {

const f32 CAMERA_VIEW_WIDTH =  100;
const f32 CAMERA_VIEW_XMIN  = -CAMERA_VIEW_WIDTH / 2.0f;
const f32 CAMERA_VIEW_XMAX  =  CAMERA_VIEW_WIDTH / 2.0f;

enum class GameViewState {
  NORMAL,
  LOOKING ///< RMB down + mouse move
};

class GameView : public QGLWidget {
  Q_OBJECT

  GameViewState      my_state;
  bool               my_navigation;
  Mat4f              my_projection;
  BasicCamera        my_camera;
  QPoint             my_last_mouse_pos;
  sptr<World>        my_world;
  sptr<Entity>       my_current_object;
  sptr<MeshTreeNode> my_origin_node;
  Vec2f              my_drag_start_pos;
  QPoint             my_cursor_pos;
  QPoint             my_ignore_mouse_move;
  bool               my_has_intersection;
  Vec3f              my_intersect_point;

public:
  GameView(const QGLFormat &format, QWidget *parent = nullptr);
  ~GameView();

  void set_core(Core *core);

  void set_world(sptr<World> world);

  void set_navigation(bool enable);

  Camera current_camera() const;

signals:
  void selection_changed(const std::vector<sptr<Entity>> &selection);

protected:
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  void keyReleaseEvent(QKeyEvent *event) override;

  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;

  void wheelEvent(QWheelEvent *event) override;

  void paintGL() override;

  void resizeGL(int w, int h) override;

private slots:
  void load();
  void unload();

private:
  Core& core();

  Vec2f widget_to_world(const QPoint &pos) const;

  void switch_state_to_normal();

  void switch_state_to_looking();

  void find_triangle_in_center();

  void draw_intersection();
};

}
}
