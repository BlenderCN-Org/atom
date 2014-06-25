#include <GL/glew.h>
#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QWheelEvent>
#include <core/math/camera.h>
#include <core/math/math.h>
#include <core/system/resource_service.h>
#include <core/processor/video_processor.h>
#include <core/video/render.h>
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
  , my_camera_pos(0, -10, 0)
  , my_camera_yaw(0)
  , my_camera_pitch(0)
{
  makeCurrent();
  setAcceptDrops(true);
  setFocusPolicy(Qt::ClickFocus);

  connect(qApp, SIGNAL(load()), SLOT(load()));
  connect(qApp, SIGNAL(unload()), SLOT(unload()));

  if (application().is_loaded()) {
    load();
  }
}

GameView::~GameView()
{

}

void GameView::set_world(sptr<World> world)
{
  if (my_world == world) {
    return;
  }

  my_world = world;

  if (my_world != nullptr) {
    update_camera_viewport();
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
  Vec2f a(my_last_mouse_pos.x(), my_last_mouse_pos.y());
  my_last_mouse_pos = event->pos();
  Vec2f b(my_last_mouse_pos.x(), my_last_mouse_pos.y());
  Vec2f delta = b - a;

  if (my_state == State::LOOKING) {
    my_camera_yaw += delta.x / 400;
    my_camera_pitch += delta.y / 400;

    Vec3f up = get_view_up(my_camera.view);
    Vec3f right = get_view_right(my_camera.view);
    Vec3f front = get_view_front(my_camera.view);

    log::info("front %s, up %s, right %s", to_cstr(front), to_cstr(up), to_cstr(right));
  }
}

void GameView::mousePressEvent(QMouseEvent *event)
{
  event->accept();

  my_last_mouse_pos = event->pos();

  if (event->button() == Qt::RightButton) {
    if (my_state == State::NORMAL) {
      switch_to(State::LOOKING);
    }
  }
}

void GameView::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton) {
    switch_to(State::NORMAL);
  }
//  QGLWidget::mouseReleaseEvent(event);
}

void GameView::wheelEvent(QWheelEvent *event)
{
  event->accept();
  update_camera_viewport();
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
    my_camera.view = calculate_basic_view(my_camera_pos, my_camera_yaw, my_camera_pitch);
//    my_camera.view = calculate_basic_view(my_camera_pos, my_camera_yaw, my_camera_pitch);
//    my_camera.set_position(my_camera_pos.x, my_camera_pos.y, my_camera_pos.z);
//  QQQ  my_camera.set_position(0, 0, 10);
//    my_camera.set_rotation(my_camera_yaw, my_camera_pitch, 0);
    my_world->set_camera(my_camera);
  }

  render_scene(core.video_service(), core.draw_service(),
    *my_world->processors().video.mesh_tree(), my_world->camera());

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
  update_camera_viewport();
}

void GameView::load()
{
  log::debug(DEBUG_EDITOR, "Loading game view");
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

void GameView::update_camera_viewport()
{
  float aspect = static_cast<f32>(width()) / height();
  my_camera.projection = Mat4f::perspective(1.57, aspect, 0.1f, 1000.0f);
}

Vec2f GameView::widget_to_world(const QPoint &pos) const
{
  not_implemented();
  return Vec2f(0, 0);
}

void GameView::switch_to(GameView::State state)
{
  my_state = state;
}

}
}

#include "editor/ui/game_view.moc"
