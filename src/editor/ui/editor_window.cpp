#include "editor_window.h"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <QSettings>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <core/world/world.h>
#include <core/system/input_service.h>
#include <core/audio/audio_service.h>
#include <core/system/resource_service.h>
#include <core/utils/json_utils.h>
#include <core/processor/debug_processor.h>
#include "ui_editor_window.h"
#include "../log.h"
#include "../ui/game_view.h"
#include "../ui/entity_list.h"
#include "../editor_application.h"
#include "../ui_utils.h"

namespace atom {
namespace editor {

Key qt_key_event_to_key(const QKeyEvent &e)
{
  switch (e.key()) {
    case Qt::Key_Left:    return Key::KEY_LEFT;
    case Qt::Key_Right:   return Key::KEY_RIGHT;
    case Qt::Key_Up:      return Key::KEY_UP;
    case Qt::Key_Down:    return Key::KEY_DOWN;
    // Qt doesn't differentiate between left/right shift key,
    // returning left shift is not correct solution because right shift will
    // interfere with it, but it is simplest temporary workaround
    case Qt::Key_Shift:   return Key::KEY_LSHIFT;
    case Qt::Key_0:       return Key::KEY_0;
    case Qt::Key_1:       return Key::KEY_1;
    case Qt::Key_2:       return Key::KEY_2;
    case Qt::Key_3:       return Key::KEY_3;
    case Qt::Key_4:       return Key::KEY_4;
    case Qt::Key_5:       return Key::KEY_5;
    case Qt::Key_6:       return Key::KEY_6;
    case Qt::Key_7:       return Key::KEY_7;
    case Qt::Key_8:       return Key::KEY_8;
    case Qt::Key_9:       return Key::KEY_9;
    case Qt::Key_A:       return Key::KEY_A;
    case Qt::Key_B:       return Key::KEY_B;
    case Qt::Key_C:       return Key::KEY_C;
    case Qt::Key_D:       return Key::KEY_D;
    case Qt::Key_E:       return Key::KEY_E;
    case Qt::Key_F:       return Key::KEY_F;
    case Qt::Key_G:       return Key::KEY_G;
    case Qt::Key_H:       return Key::KEY_H;
    case Qt::Key_I:       return Key::KEY_I;
    case Qt::Key_J:       return Key::KEY_J;
    case Qt::Key_K:       return Key::KEY_K;
    case Qt::Key_L:       return Key::KEY_L;
    case Qt::Key_M:       return Key::KEY_M;
    case Qt::Key_N:       return Key::KEY_N;
    case Qt::Key_O:       return Key::KEY_O;
    case Qt::Key_P:       return Key::KEY_P;
    case Qt::Key_Q:       return Key::KEY_Q;
    case Qt::Key_R:       return Key::KEY_R;
    case Qt::Key_S:       return Key::KEY_S;
    case Qt::Key_T:       return Key::KEY_T;
    case Qt::Key_U:       return Key::KEY_U;
    case Qt::Key_V:       return Key::KEY_V;
    case Qt::Key_W:       return Key::KEY_W;
    case Qt::Key_X:       return Key::KEY_X;
    case Qt::Key_Y:       return Key::KEY_Y;
    case Qt::Key_Z:       return Key::KEY_Z;
    case Qt::Key_Control:
//      log::info("Native %i, virtual %i", e.nativeScanCode(), e.nativeVirtualKey());
      return e.nativeVirtualKey() == 65508 ? Key::KEY_RCTRL : Key::KEY_LCTRL;

  }

  return Key::KEY_UNKNOWN;
}

Key qt_mouse_button_to_key(const QMouseEvent &e)
{
  switch (e.button()) {
    case Qt::LeftButton:
      return Key::KEY_LMB;

    case Qt::MiddleButton:
      return Key::KEY_MMB;

    case Qt::RightButton:
      return Key::KEY_RMB;

    default:
      break;
  }

  return Key::KEY_UNKNOWN;
}

sptr<Entity> create_entity(const std::vector<EntityCreator> &creators, const String &class_name,
  World &world, Core &core)
{
  auto found = std::find_if(creators.begin(), creators.end(),
    [&class_name](const EntityCreator &creator) { return creator.name == class_name; });

  if (found == creators.end()) {
    return nullptr;
  }

  sptr<Entity> entity = found->create(world, core);
  entity->set_class_name(class_name);
  return entity;
}

bool save_to_file(const QString &filename, const World &world)
{
  assert(!filename.isEmpty());

  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Value entity_array;
  entity_array.SetArray();

  for (const sptr<Entity> &o : world.objects()) {
    log::debug(DEBUG_EDITOR_SAVE, "Saving entity id=\"%s\", class=\"%s\"",
      o->id().c_str(), o->class_name().c_str());
    rapidjson::Value obj_json;
    obj_json.SetObject();

    const MetaClass &meta_class = *o->meta;

    MetaFieldsEnumarator fields(meta_class);
    const MetaField *field;

    while ((field = fields.next())) {
      log::debug(DEBUG_EDITOR_SAVE, "Saving entity field \"%s\"", field->name);
      utils::write_basic_property_to_json(doc, obj_json, *field, o.get());
    }

    entity_array.PushBack(obj_json, doc.GetAllocator());
  }

  doc.AddMember("entities", entity_array, doc.GetAllocator());

  std::ofstream output(filename.toLatin1().constData());
  utils::JsonOutputStream stream(output);

  rapidjson::PrettyWriter<utils::JsonOutputStream> writer(stream);

  doc.Accept(writer);
  return true;
}

bool load_from_file(const QString &filename, Core &core, World &world)
{
  world.clear();

  std::ifstream input(filename.toLatin1());

  if (!input.is_open()) {
    log::warning("Can't open file \"%s\"", filename.toLatin1().constData());
    return false;
  }

  utils::JsonInputStream stream(input);
  rapidjson::Document doc;
  doc.ParseStream<0>(stream);

  if (doc.HasParseError()) {
    log::error("%s Offset %i", doc.GetParseError(), doc.GetErrorOffset());
    return false;
  }

  if (!doc.IsObject()) {
    log::error("Top level element must be object");
    return false;
  }

  if (!doc.HasMember("entities")) {
    log::error("Level file doesn't contain entities");
    return false;
  }

  const rapidjson::Value &entities = doc["entities"];

  u32 count = entities.Size();

  log::debug(DEBUG_EDITOR_LOAD, "Loading entities (%u)", count);

  for (uint i = 0; i < count; ++i) {
    const rapidjson::Value &obj = entities[i];
    // each entity must be object
    if (!obj.IsObject()) {
      log::error("Entity must be object, skipping");
      continue;
    }
    // each entity class is identified by "class" field
    if (!obj.HasMember("class")) {
      log::error("Entity missing \"class\" field, skipping");
      continue;
    }

    const rapidjson::Value &class_name = obj["class"];

    if (!class_name.IsString()) {
      log::error("Entity \"class\" must be string, skipping");
      continue;
    }

    const String entity_class(class_name.GetString());

    sptr<Entity> entity = create_entity(core.entity_creators(), entity_class, world, core);

    if (entity == nullptr) {
      log::warning("Unknown entity class \"%s\"", entity_class.c_str());
      continue;
    }

    // load properties
    MetaFieldsEnumarator enumerator(*entity->meta);
    const MetaField *field;

    while ((field = enumerator.next())) {
      log::debug(DEBUG_EDITOR_LOAD, "Loading entity field \"%s\"", field->name);
      utils::ReadResult result = utils::read_basic_property_from_json(obj, *field, entity.get());

      if (result != utils::ReadResult::OK) {
        log::warning("Can't read field \"%s\" of entity \"%s\"", field->name, entity_class.c_str());
      }
    }

    world.add_entity(entity);
  }

  return true;
}

EditorWindow::EditorWindow()
  : QMainWindow()
  , my_ui(new Ui::MainWindow())
  , my_mode(EditorWindowMode::EDIT)
  , my_update_interval(stf(1) * 1000)
  , my_entity_list(nullptr)
  , my_entity_edit(nullptr)
  , my_undo_view(nullptr)
  , my_undo_action(nullptr)
  , my_redo_action(nullptr)
{
  my_ui->setupUi(this);
  init_game_view();
  init_undo_view();
  init_menu_view();
  my_entity_list = my_ui->entity_list;
  my_entity_edit = my_ui->entity_edit;

#ifdef _WIN32
  my_last_update = milli_time();
#endif

  connect(&my_refresh_timer, SIGNAL(timeout()), SLOT(update_world()));
  my_refresh_timer.start(my_update_interval);

  restore_geometry();

  connect(qApp, SIGNAL(load()), SLOT(load()));
  connect(qApp, SIGNAL(unload()), SLOT(unload()));

  if (application().is_loaded()) {
    load();
  }
}

EditorWindow::~EditorWindow()
{
  switch_mode(EditorWindowMode::EDIT);
}

void EditorWindow::on_action_quit_triggered()
{
  log::debug(DEBUG_EDITOR, "Action quit");
  close();
}

void EditorWindow::update_world()
{
  EditorApplication &app = application();
  app.core().update();
  app.core().input_service().poll();

  switch (my_mode) {
    case EditorWindowMode::EDIT:
      app.world()->step();
      break;

    case EditorWindowMode::GAME:
      my_clone->step();
      break;
  }

  my_game_view->update();

#ifdef _WIN32
  // tune update interval to match desired FPS
  i64 now = milli_time();
  i64 duration = now - my_last_update;
  i64 delta = (1000 / FPS - duration);

//  std::cout << "Frame duration " << (now - my_last_update) << ", delta " << delta
//            << ", update interval " << my_update_interval << std::endl;

  my_last_update = now;
  my_update_interval = std::max<i64>(0, my_update_interval + delta / 10);
  my_refresh_timer.start(my_update_interval);
#endif
}

void EditorWindow::switch_mode(EditorWindowMode mode)
{
  // mode is actual mode so do nothing
  if (mode == my_mode) {
    return;
  }
  // initialize edit mode
  if (mode == EditorWindowMode::EDIT) {
    log::debug(DEBUG_EDITOR, "Switching to EDIT mode");
    application().core().audio_service().clear();
    my_game_view->set_world(application().world());
    // destroy cloned world
    if (my_clone != nullptr) {
      my_clone->deactivate();
      my_clone.reset();
    }
    // restore standard input event flow
    my_game_view->removeEventFilter(this);
  }
  // initialize game mode
  if (mode == EditorWindowMode::GAME) {
    log::debug(DEBUG_EDITOR, "Switching to GAME mode");
    // save panels visibility state
    my_panels.entity_list = my_ui->entity_list_dock->isVisible();
    my_panels.entity_edit = my_ui->entity_edit_dock->isVisible();
    my_panels.undo_view   = my_ui->undo_view_dock->isVisible();
    // make clone of active world
    my_clone = application().world()->clone();
    my_clone->activate();
    my_game_view->set_world(my_clone);
    my_game_view->set_camera_free_look(false);
    // redirect input events to InputService
    my_game_view->installEventFilter(this);
    my_game_view->setFocus();
    center_cursor(*my_game_view);
  }

  bool edit_mode = mode == EditorWindowMode::EDIT;
  bool game_mode = mode == EditorWindowMode::GAME;
  // receive mouse move events without need of pressed button
  my_game_view->setMouseTracking(game_mode);
  // show/hide mouse cursor
  my_game_view->setCursor(game_mode ? Qt::BlankCursor : Qt::ArrowCursor);
  my_ui->action_run->setText(mode == EditorWindowMode::EDIT ? "Run" : "Stop");
  // disable double click in game mode
  application().setDoubleClickInterval(edit_mode ? 400 : 0);
  // enable/disable camera navigation
  my_game_view->set_camera_free_look(edit_mode);
  // hide editor panels
  my_ui->entity_list_dock->setVisible(edit_mode && my_panels.entity_list);
  my_ui->entity_edit_dock->setVisible(edit_mode && my_panels.entity_edit);
  my_ui->undo_view_dock->setVisible(edit_mode && my_panels.undo_view);
  // disable edit action in game mode
  my_ui->action_open->setEnabled(edit_mode);
  my_ui->action_save->setEnabled(edit_mode);
  my_ui->action_save_as->setEnabled(edit_mode);
  my_ui->menu_edit->setEnabled(edit_mode);
  my_ui->menu_panels->setEnabled(edit_mode);

  my_mode = mode;
}

void EditorWindow::init_menu_view()
{
  my_ui->menu_panels->addAction(my_ui->entity_list_dock->toggleViewAction());
  my_ui->menu_panels->addAction(my_ui->entity_edit_dock->toggleViewAction());
  my_ui->menu_panels->addAction(my_ui->undo_view_dock->toggleViewAction());
}

void EditorWindow::init_game_view()
{
  // init opengl
  QGLFormat gl_format;
  gl_format.setVersion(4, 2);
  gl_format.setProfile(QGLFormat::CompatibilityProfile);
  gl_format.setDoubleBuffer(true);
  gl_format.setRgba(true);
  gl_format.setRedBufferSize(8);
  gl_format.setGreenBufferSize(8);
  gl_format.setBlueBufferSize(8);
  gl_format.setAlphaBufferSize(8);
  gl_format.setStencil(true);
  gl_format.setAccum(true);

  my_game_view = new GameView(gl_format, this);

  setCentralWidget(my_game_view);

  connect(my_game_view, SIGNAL(selection_changed(const std::vector<sptr<Entity>> &)),
    SLOT(selection_changed(const std::vector<sptr<Entity>> &)));
}

void EditorWindow::init_undo_view()
{
  my_undo_view = new QUndoView(this);
  my_ui->undo_view_dock->setWidget(my_undo_view);
}

void EditorWindow::add_undo_redo()
{
  assert(my_undo_action == nullptr);
  assert(my_redo_action == nullptr);
  QUndoStack &undo_stack = application().undo_stack();
  // create undo action
  my_undo_action = undo_stack.createUndoAction(this);
  my_undo_action->setShortcut(QKeySequence::Undo);
  my_ui->menu_edit->addAction(my_undo_action);
  // create redo action
  my_redo_action = undo_stack.createRedoAction(this);
  my_redo_action->setShortcut(QKeySequence::Redo);
  my_ui->menu_edit->addAction(my_redo_action);

  connect(&undo_stack, SIGNAL(cleanChanged(bool)), SLOT(clean_changed(bool)));
}

void EditorWindow::remove_undo_redo()
{
  assert(my_undo_action != nullptr);
  assert(my_redo_action != nullptr);

  my_undo_action->deleteLater();
  my_undo_action = nullptr;

  my_redo_action->deleteLater();
  my_redo_action = nullptr;

  disconnect(&application().undo_stack(), SIGNAL(cleanChanged(bool)), this, SLOT(clean_changed(bool)));
}

void EditorWindow::restore_geometry()
{
  // restore geometry & state
  QSettings settings;
  restoreGeometry(settings.value("geometry").toByteArray());
  restoreState(settings.value("window_state").toByteArray());
}

void EditorWindow::save_geometry()
{
  switch_mode(EditorWindowMode::EDIT);

  QSettings settings;
  settings.setValue("geometry", saveGeometry());
  settings.setValue("window_state", saveState());
}

QString EditorWindow::make_window_title(bool clean) const
{
  return clean ? document_name() : document_name() + "*";
}

QString EditorWindow::document_name() const
{
  return my_filename.isEmpty() ? "Untitled" : my_filename;
}

bool EditorWindow::eventFilterKey(QKeyEvent &event)
{
  // ignore auto generated event
  if (event.isAutoRepeat()) {
    return false;
  }

  Key key = qt_key_event_to_key(event);

  if (key == Key::KEY_UNKNOWN) {
    return false;
  }

  event.accept();
  application().core().input_service().push_event(
    make_key_event(key, event.type() == QEvent::KeyPress));

  return true;
}

bool EditorWindow::eventFilterMouseKey(QMouseEvent &event)
{
  event.accept();

  Key key = qt_mouse_button_to_key(event);

  if (key != Key::KEY_UNKNOWN) {
    application().core().input_service().push_event(
      make_key_event(key, event.type() == QEvent::MouseButtonPress));
  }

  return true;
}

bool EditorWindow::eventFilterMouseMove(QMouseEvent &event)
{
  event.accept();
  QPoint center = my_game_view->mapToGlobal(get_widget_center(*my_game_view));
  QPoint delta = event.globalPos() - center;

  if (delta.x() == 0 && delta.y() == 0) {
    return true;
  }

  application().core().input_service().push_event(
    make_mouse_event(delta.x() / 100.0f, -delta.y() / 100.0f));

  center_cursor(*my_game_view);
  return true;
}

void EditorWindow::on_action_run_triggered()
{
  log::debug(DEBUG_EDITOR, "Action run");

  if (my_mode == EditorWindowMode::EDIT) {
    switch_mode(EditorWindowMode::GAME);
  } else if (my_mode == EditorWindowMode::GAME) {
    switch_mode(EditorWindowMode::EDIT);
  } else {
    log::error("Unknown editor mode");
  }
}

void EditorWindow::on_action_show_physics_triggered(bool checked)
{
  application().world()->processors().debug.set_debug(DebugCategory::PHYSICS, checked);
}

void EditorWindow::on_action_show_bounding_box_triggered(bool checked)
{
  application().world()->processors().debug.set_debug(DebugCategory::BOUNDING_BOX, checked);
}

void EditorWindow::on_action_show_aabb_triggered(bool checked)
{
  application().world()->processors().debug.set_debug(DebugCategory::AABB, checked);
}

void EditorWindow::on_action_show_geometry_triggered(bool checked)
{
  application().world()->processors().debug.set_debug(DebugCategory::GEOMETRY_CACHE, checked);
}

void EditorWindow::load()
{
  log::debug(DEBUG_EDITOR, "Loading editor window");
  add_undo_redo();
  my_game_view->set_world(application().world());
}

void EditorWindow::unload()
{
  log::debug(DEBUG_EDITOR, "Unloading editor window");
  remove_undo_redo();
  my_clone.reset();
}

void EditorWindow::selection_changed(const std::vector<sptr<Entity> > &selection)
{
  assert(my_entity_edit != nullptr);
  my_entity_edit->set_entity(selection.empty() ? nullptr : selection[0]);
}

void EditorWindow::clean_changed(bool clean)
{
  setWindowTitle(make_window_title(clean));
}

void EditorWindow::closeEvent(QCloseEvent *event)
{
  save_geometry();
  QMainWindow::closeEvent(event);
}

void EditorWindow::on_action_file_new_triggered()
{
  application().world()->clear();
  my_filename.clear();
}

void EditorWindow::on_action_open_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open level file", LEVEL_RESOURCE_DIR);

  if (filename.isEmpty()) {
    return;
  }

  load_file(filename);
}

void EditorWindow::on_action_save_triggered()
{
  log::debug(DEBUG_EDITOR, "Save action");

  if (my_filename.isEmpty()) {
    on_action_save_as_triggered();
  } else {
    save_to_file(my_filename, *application().world());
    application().undo_stack().setClean();
  }
}

void EditorWindow::on_action_save_as_triggered()
{
  log::debug(DEBUG_EDITOR, "Save as action");

  QString file = QFileDialog::getSaveFileName(this, "Save to file", LEVEL_RESOURCE_DIR,
    tr("Level files (*.lev);;All files (*.*)"));

  if (file.isEmpty()) {
    return;
  }

  my_filename = file;
  save_to_file(my_filename, *application().world());
  undo_stack().setClean();
}

bool EditorWindow::eventFilter(QObject *watched, QEvent *event)
{
  assert(watched == my_game_view);

  switch (event->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
      return eventFilterKey(*static_cast<QKeyEvent *>(event));

    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
      return eventFilterMouseKey(*static_cast<QMouseEvent *>(event));

    case QEvent::MouseMove:
      return eventFilterMouseMove(*static_cast<QMouseEvent *>(event));

    default:
      break;
  }

  return false;
}

void EditorWindow::load_file(const QString &filename)
{
  World &world = *application().world();
  Core &core = application().core();

  // erase old scene
  /// @todo scene should be erased only after the load is successful
  world.clear();
  undo_stack().clear();

  // process selected file
  if (!load_from_file(filename, core, world)) {
    QMessageBox::warning(this, "Error opening file",
      QString("Error while loading world file \"%1\".\nSee console log for more details.").arg(filename));
  }

  my_filename = filename;
}

}
}

#include "editor_window.moc"
