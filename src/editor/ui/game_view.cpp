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

const f32 GameView::ZOOM_FACTORS[] = { 400, 200, 100, 50, 25, 10, 5, 2 };
const int GameView::ZOOM_FACTORS_SIZE = sizeof(ZOOM_FACTORS) / sizeof(ZOOM_FACTORS[0]);

GameView::GameView(const QGLFormat &format, QWidget *parent)
  : QGLWidget(format, parent)
  , my_navigation(true)
  , my_zoom(4)
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
        entity->set_position(widget_to_world(event->pos()));
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
    my_current_object->set_position(my_drag_start_pos);
//    QQQmy_current_object->reboot();
    my_current_object.reset();
    event->accept();
    return;
  }

  QGLWidget::keyReleaseEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
  QPoint last_pos = my_last_mouse_pos;
  my_last_mouse_pos = event->pos();

  if (event->buttons() & Qt::MidButton) {
    QPoint d = last_pos - event->pos();
    f32 zoom = ZOOM_FACTORS[my_zoom];
    f32 w = width() / zoom;
    f32 h = height() / zoom;
    f32 dx = (static_cast<f32>(d.x()) /  width()) * w;
    f32 dy = (static_cast<f32>(d.y()) / height()) * h;

    my_camera.translate(dx, -dy, 0);

  } else if (event->buttons() & Qt::LeftButton) {
    if (my_current_object == nullptr) {
      return;
    }

    QPoint d = event->pos() - last_pos;

    f32 zoom = ZOOM_FACTORS[my_zoom];
    f32 w = width() / zoom;
    f32 h = height() / zoom;
    f32 dx = (static_cast<f32>(d.x()) /  width()) * w;
    f32 dy = (static_cast<f32>(d.y()) / height()) * h;

    Vec2f position = my_current_object->position() + Vec2f(dx, -dy);
    my_current_object->set_position(position);
  }
}

void GameView::mousePressEvent(QMouseEvent *event)
{
  event->accept();

  my_last_mouse_pos = event->pos();

  if (event->buttons() & Qt::LeftButton) {
    Vec2f pos = widget_to_world(my_last_mouse_pos);
    my_current_object = application().world()->find_entity(pos);

    std::vector<sptr<Entity>> selection;

    if (my_current_object != nullptr) {
      my_drag_start_pos = my_current_object->position();
      selection.push_back(my_current_object);
    }

    log::info("Object at %s is %s", to_cstr(pos), my_current_object != nullptr ? "found" : "null");

    emit selection_changed(selection);
  }

//  QGLWidget::mousePressEvent(event);
}

void GameView::mouseReleaseEvent(QMouseEvent *event)
{
  if (event->button() & Qt::LeftButton) {
    if (my_current_object != nullptr && my_drag_start_pos != my_current_object->position()) {
      //create move command
      EntityMove *command = new EntityMove(my_current_object, my_drag_start_pos,
        my_current_object->position());
      undo_stack().push(command);
      log::info("Creating move command");
    }
    my_current_object.reset();  //QQQ pridat metodu na uvolnenie vsetkych engine zdrojov, potrebne pri reloade aj vypinani
  }

  QGLWidget::mouseReleaseEvent(event);
}

void GameView::wheelEvent(QWheelEvent *event)
{
  event->accept();

  my_zoom += event->delta() < 0 ? 1 : -1;
  my_zoom = range(0, ZOOM_FACTORS_SIZE - 1, my_zoom);

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
  f32 zoom = ZOOM_FACTORS[my_zoom];
  f32 w = width() / zoom;
  f32 h = height() / zoom;

  //  float aspect = static_cast<f32>(width()) / height();
  my_camera.set_orthographic(-w/2, w/2, -h/2, h/2, -5, 5);
}

Vec2f GameView::widget_to_world(const QPoint &pos) const
{
  f32 zoom = ZOOM_FACTORS[my_zoom];

  // 0..1
  f32 wx = static_cast<f32>(pos.x()) / width();
  f32 wy = static_cast<f32>(height() - pos.y() - 1) / height();
  // calculate camera view width/height
  f32 w = width() / zoom;
  f32 h = height() / zoom;
  // -camera_w..camera_w, -camera_h..camera_h
  f32 sx = lerp(wx, -w/2, w/2);
  f32 sy = lerp(wy, -h/2, h/2);
  // transform point with camera view
  Vec4f p = my_camera.view_matrix().inverted() * Vec4f(sx, sy, 0, 1);
  return Vec2f(p.x, p.y);
}

}
}

#include "editor/ui/game_view.moc"
