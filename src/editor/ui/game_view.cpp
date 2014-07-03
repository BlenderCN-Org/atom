#include <GL/glew.h>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QWheelEvent>
#include <core/math/camera.h>
#include <core/math/math.h>
#include <core/system/resource_service.h>
#include <core/processor/video_processor.h>
#include <core/video/draw_service.h>
#include <core/utils/utils.h>
#include <core/world/world.h>
#include "editor/editor_application.h"
#include "editor/commands/entity_move.h"
#include "editor/commands/entity_add.h"
#include "editor/log.h"
#include "game_view.h"

namespace atom {
namespace editor {

GameView::GameView(const QGLFormat &format, QWidget *parent)
  : QGLWidget(format, parent)
  , my_state(State::NORMAL)
  , my_navigation(true)
{
  makeCurrent();
  setAcceptDrops(true);
  setFocusPolicy(Qt::ClickFocus);

  my_camera.set_position(Vec3f(0, 0, 0));

  connect(qApp, SIGNAL(load()), SLOT(load()));
  connect(qApp, SIGNAL(unload()), SLOT(unload()));

  if (application().is_loaded()) {
    load();
  }
}

GameView::~GameView()
{
  switch_state_to_normal();
  my_world = nullptr;
  my_current_object = nullptr;
  my_origin_node = nullptr;
}

void GameView::set_world(sptr<World> world)
{
  if (my_world == world) {
    return;
  }

  my_world = world;

  if (my_world != nullptr) {
    my_world->processors().video.set_resolution(width(), height());
  }
}

void GameView::set_navigation(bool enable)
{
  my_navigation = enable;
}

void GameView::dragEnterEvent(QDragEnterEvent *event)
{
  log::debug(DEBUG_EDITOR, "Drag action for mime format %s",
    event->mimeData()->formats().join(" ").toStdString().c_str());

  if (event->mimeData()->hasFormat("mm/object")) {
    log::debug(DEBUG_EDITOR, "Accepting drag");
    event->acceptProposedAction();
  }
}

void GameView::dropEvent(QDropEvent *event)
{
  if (my_world == nullptr) {
    return;
  }

  if (event->mimeData()->hasFormat("mm/object")) {
    log::debug(DEBUG_EDITOR, "Drop accepted");

    const QMimeData *mime_data = event->mimeData();

    if (mime_data->hasFormat("mm/object")) {
      event->acceptProposedAction();
      QString object_type(mime_data->data("mm/object"));
      log::debug(DEBUG_EDITOR, "Drop event %s", object_type.toLatin1().data());

      bool ok = false;
      uint index = object_type.toLatin1().toInt(&ok);

      if (index >= application().core().entity_creators().size()) {
        log::warning("Invalid object index %i, max index is %i", index,
          application().core().entity_creators().size());
        return;
      }

      if (!ok) {
        return;
      }

      sptr<Entity> entity = application().core().entity_creators()[index].create(*my_world, application().core());

      if (entity != nullptr) {
        entity->set_class_name(application().core().entity_creators()[index].name);
        log::info("Adding entity to the world");
//        entity->set_position(widget_to_world(event->pos()));
//        QQQentity->init();

        EntityAdd *command = new EntityAdd(entity, application().world());
        undo_stack().push(command);
      } else {
        log::warning("Error while creating object");
      }
    } else {
      log::debug(DEBUG_EDITOR, "Drop ignored");
      event->ignore();
    }
  }
}

void GameView::keyReleaseEvent(QKeyEvent *event)
{
  if (my_current_object != nullptr && QApplication::mouseButtons() & Qt::LeftButton) {
//    my_current_object->set_position(my_drag_start_pos);
//    QQQmy_current_object->reboot();
//    my_current_object.reset();
    event->accept();
    return;
  }

  QGLWidget::keyReleaseEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
  QPoint delta = event->pos() - my_last_mouse_pos;

  if (my_state == State::LOOKING) {
    if (delta == my_ignore_mouse_move) {
      return;
    }

    my_camera.set_yaw(my_camera.get_yaw() + delta.x() / 400.0f);
    my_camera.set_pitch(my_camera.get_pitch() - delta.y() / 400.0f);

    my_ignore_mouse_move = event->pos() - mapFromGlobal(my_cursor_pos);
    QCursor::setPos(my_cursor_pos);
  }
}

void GameView::mousePressEvent(QMouseEvent *event)
{
  event->accept();

  my_last_mouse_pos = event->pos();

  if (event->button() == Qt::RightButton) {
    if (my_state == State::NORMAL) {
      switch_state_to_looking();
    }
  }
}

void GameView::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton) {
    switch_state_to_normal();
  }
//  QGLWidget::mouseReleaseEvent(event);
}

void GameView::wheelEvent(QWheelEvent *event)
{
  event->accept();

  float degrees = event->delta() / 8.0f;
  Vec3f pos = my_camera.get_position() + my_camera.get_front() * degrees / 10.0f;
  my_camera.set_position(pos);
}

void GameView::paintGL()
{
  Core &core = application().core();
  GBuffer &gbuffer = my_world->processors().video.get_gbuffer();

  core.video_service().bind_write_framebuffer(gbuffer.get_render_framebuffer());

  if (my_world == nullptr) {
    return;
  }

  core.resource_service().poll();

  glClearColor(0.09, 0.17, 0.27, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (my_navigation) {
    Camera camera;
    camera.view = my_camera.get_view_matrix();
    camera.projection = my_projection;
    my_world->set_camera(camera);
  }
  
  my_world->processors().video.render(my_world->camera());

  core.video_service().unbind_write_framebuffer();
  my_world->processors().video.get_gbuffer().blit();
}

void GameView::resizeGL(int w, int h)
{
  Config::instance().set_screen_resolution(w, h);

  if (my_world != nullptr) {
    my_world->processors().video.set_resolution(w, h);
  }

  glViewport(0, 0, w, h);
  float aspect = static_cast<f32>(width()) / height();
  my_projection = Mat4f::perspective(1.57, aspect, 0.1f, 1000.0f);
}

void GameView::load()
{
  log::debug(DEBUG_EDITOR, "Loading game view");
  core().resource_service().get_mesh("compound");
  core().resource_service().get_material("test2");
}

void GameView::unload()
{
  log::debug(DEBUG_EDITOR, "Unloading game view");
  my_world.reset();
  my_origin_node.reset();
  my_current_object.reset();
}

Core &GameView::core()
{
  return application().core();
}

Vec2f GameView::widget_to_world(const QPoint &pos) const
{
  not_implemented();
  return Vec2f(0, 0);
}

void GameView::switch_state_to_normal()
{
  if (my_state == State::NORMAL) {
    return;
  }

  releaseMouse();
  QCursor::setPos(my_cursor_pos);
  my_state = State::NORMAL;

}

void GameView::switch_state_to_looking()
{
  if (my_state == State::LOOKING) {
    return;
  }

  my_cursor_pos = QCursor::pos();
  grabMouse(QCursor(Qt::BlankCursor));
  my_state = State::LOOKING;
}

}
}

#include "editor/ui/game_view.moc"
