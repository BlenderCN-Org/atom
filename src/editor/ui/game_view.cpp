#include <QApplication>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QWheelEvent>
#include <core/math/camera.h>
#include <core/math/math.h>
#include <core/system/resource_service.h>
#include <core/processor/render_processor.h>
#include <core/processor/physics_processor.h>
#include <core/processor/debug_processor.h>
#include <core/processor/geometry_processor.h>
#include <core/video/draw_service.h>
#include <core/video/model.h>
#include <core/video/render_context.h>
#include <core/component/model_component.h>
#include <core/utils/utils.h>
#include <core/world/world.h>
#include <core/math/intersect.h>
#include <core/video/uniforms.h>
#include "../editor_application.h"
#include "../commands/entity_move.h"
#include "../commands/entity_add.h"
#include "../log.h"
#include "game_view.h"

namespace atom {
namespace editor {

GameView::GameView(const QGLFormat &format, QWidget *parent)
  : QGLWidget(format, parent)
  , my_state(GameViewState::NORMAL)
  , my_is_free_look_enabled(true)
  , my_has_intersection(false)
{
  makeCurrent();
  setAcceptDrops(true);
  setFocusPolicy(Qt::ClickFocus);
  // initial camera position
  my_camera.set_position(Vec3f(0, -5, 5));

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

void GameView::set_camera_free_look(bool enable)
{
  my_is_free_look_enabled = enable;
}

Camera GameView::current_camera() const
{
  Camera camera;
  camera.view = my_camera.get_view_matrix();
  camera.projection = my_projection;
  return camera;
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
      u32 index = object_type.toLatin1().toInt(&ok);

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
    event->accept();
    return;
  }

  QGLWidget::keyReleaseEvent(event);
}

void GameView::mouseMoveEvent(QMouseEvent *event)
{
  event->accept();
  QPoint delta = event->pos() - my_last_mouse_pos;

  if (my_state == GameViewState::LOOKING) {
    if (delta == my_ignore_mouse_move) {
      return;
    }

    my_camera.set_yaw(my_camera.get_yaw() + delta.x() / 400.0f);
    my_camera.set_pitch(my_camera.get_pitch() - delta.y() / 400.0f);

    my_ignore_mouse_move = event->pos() - mapFromGlobal(my_cursor_pos);
    QCursor::setPos(my_cursor_pos);

    find_triangle_in_center();
  }
}

void GameView::mousePressEvent(QMouseEvent *event)
{
  event->accept();

  my_last_mouse_pos = event->pos();

  if (event->button() == Qt::RightButton) {
    if (my_state == GameViewState::NORMAL) {
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

  if (my_is_free_look_enabled) {
    my_world->set_camera(current_camera());
  }

  my_world->processors().video.render(my_world->camera());

  core.video_service().disable_depth_test();

  my_world->processors().debug.draw();

  core.video_service().enable_depth_test();

  if (my_has_intersection) {
    //draw intersection line
    draw_intersection();
  }

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
  if (my_state == GameViewState::NORMAL) {
    return;
  }

  releaseMouse();
  QCursor::setPos(my_cursor_pos);
  my_state = GameViewState::NORMAL;

}

void GameView::switch_state_to_looking()
{
  if (my_state == GameViewState::LOOKING) {
    return;
  }

  my_cursor_pos = QCursor::pos();
  grabMouse(QCursor(Qt::BlankCursor));
  my_state = GameViewState::LOOKING;
}

void GameView::find_triangle_in_center()
{
  RayGeometryResult result;
  Ray ray(my_camera.get_position(), my_camera.get_front());
  my_has_intersection = my_world->processors().geometry.intersect_ray(ray, U32_MAX, result);
  
  
  if (my_has_intersection) {
    my_intersect_point = result.hit;
  }
}

void GameView::draw_intersection()
{
  std::vector<Vec3f> lines;
  lines.push_back(my_intersect_point);
  lines.push_back(my_intersect_point + Vec3f(0, 0, 5));

  Mesh mesh;
  VideoService &vs = core().video_service();
  uptr<VideoBuffer> vertex_buffer(new VideoBuffer(vs, VideoBufferUsage::STATIC_DRAW));
  vertex_buffer->set_bytes(lines.data(), lines.size() * sizeof(Vec3f));

  Uniforms &u = vs.get_uniforms();
  u.transformations.model = Mat4f();
  u.model = Mat4f();
  u.mvp = u.transformations.model_view_projection();
  RenderContext context = { u, vs };

  mesh.vertex = std::move(vertex_buffer);
  MaterialResourcePtr material = core().resource_service().get_material("lines");
  material->material().draw_mesh(context, mesh);
}

}
}

#include "editor/ui/game_view.moc"
