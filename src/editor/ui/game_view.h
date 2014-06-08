#pragma once

#include <QGLWidget>
#include <QTimer>
#include <core/corefwd.h>
#include "editor/editorfwd.h"

namespace atom {
namespace editor {

const f32 CAMERA_VIEW_WIDTH =  100;
const f32 CAMERA_VIEW_XMIN  = -CAMERA_VIEW_WIDTH / 2.0f;
const f32 CAMERA_VIEW_XMAX  =  CAMERA_VIEW_WIDTH / 2.0f;

class GameView : public QGLWidget {
  Q_OBJECT
public:

  static const f32 ZOOM_FACTORS[];// = { 400, 200, 100, 50, 25, 10, 5, 2 };
  static const int ZOOM_FACTORS_SIZE;// = sizeof(ZOOM_FACTORS) / sizeof(ZOOM_FACTORS[0]);

  GameView(const QGLFormat &format, QWidget *parent = nullptr);
  ~GameView();

  void set_core(Core *core);

  void set_world(sptr<World> world);

  void set_navigation(bool enable);

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

  void update_camera_viewport();

  Vec2f widget_to_world(const QPoint &pos) const;

private:
  bool               my_navigation;
  Camera             my_camera;
  Vec2f              my_camera_pos;
  QPoint             my_last_mouse_pos;
  sptr<World>        my_world;
  sptr<Entity>       my_current_object;
  sptr<MeshTreeNode> my_origin_node;
  Vec2f              my_drag_start_pos;
  int                my_zoom;  ///< zoom index
};

}
}
