#include "editor_window.h"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <QSettings>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <core/world/world.h>
#include <core/input/input_service.h>
#include <core/audio/audio_service.h>
#include <core/system/resource_service.h>
#include <core/utils/json_utils.h>
#include "editor/log.h"
#include "editor/ui/game_view.h"
#include "editor/ui/entity_list.h"
#include "editor/ui/ui_editor_window.h"
#include "editor/editor_application.h"
#include "editor/ui_utils.h"

namespace atom {
namespace editor {

Key qt_key_event_to_key(const QKeyEvent &e)
{
  switch (e.key()) {
    case Qt::Key_Left:    return Key::KEY_LEFT;
    case Qt::Key_Right:   return Key::KEY_RIGHT;
    case Qt::Key_Up:      return Key::KEY_UP;
    case Qt::Key_Down:    return Key::KEY_DOWN;
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

bool load_from_file(const QString &filename, World &world,
  const std::vector<EntityCreator> &creators, Core &core)
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
    log::error("Error while parsing \"%s\"", doc.GetParseError());
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

  uint count = entities.Size();

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

    sptr<Entity> entity = create_entity(creators, entity_class, world, core);

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
  , my_mode(Mode::EDIT)
  , my_update_interval(stf(1) * 1000)
  , my_entity_list(nullptr)
  , my_entity_edit(nullptr)
  , my_undo_view(nullptr)
  , my_undo_action(nullptr)
  , my_redo_action(nullptr)
  , my_grab_input(false)
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
  // release grabbed input
  my_game_view->releaseKeyboard();
  my_game_view->releaseMouse();
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
    case Mode::EDIT:
      app.world()->step();
      break;

    case Mode::GAME:
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

void EditorWindow::switch_mode(EditorWindow::Mode mode)
{
  if (mode == my_mode) {
    return;
  }

  switch (mode) {
    case Mode::EDIT:
      application().core().audio_service().clear();
      log::debug(DEBUG_EDITOR, "Switching to EDIT mode");
      my_ui->action_run->setText("Run");

      my_ui->entity_list_dock->setVisible(my_dock_visibility.entity_list);
      my_ui->entity_edit_dock->setVisible(my_dock_visibility.entity_edit);
      my_ui->undo_view_dock->setVisible(my_dock_visibility.undo_view);

      my_game_view->set_world(application().world());
      my_clone.reset();
      my_game_view->setMouseTracking(false);
      my_game_view->releaseMouse();
      my_game_view->removeEventFilter(this);
      my_game_view->set_navigation(true);
      application().setDoubleClickInterval(400);

      if (my_grab_input) {
        my_game_view->releaseKeyboard();
      }
      break;

    case Mode::GAME:
      log::debug(DEBUG_EDITOR, "Switching to GAME mode");
      my_ui->action_run->setText("Stop");

      my_dock_visibility.entity_list = my_ui->entity_list_dock->isVisible();
      my_dock_visibility.entity_edit = my_ui->entity_edit_dock->isVisible();
      my_dock_visibility.undo_view   = my_ui->undo_view_dock->isVisible();

      my_ui->entity_list_dock->setVisible(false);
      my_ui->entity_edit_dock->setVisible(false);
      my_ui->undo_view_dock->setVisible(false);

      my_clone = application().world()->clone();
      my_clone->wake_up();
      my_game_view->set_world(my_clone);
      my_game_view->set_navigation(false);
      my_game_view->grabMouse(Qt::BlankCursor);
      application().setDoubleClickInterval(0);

      if (my_grab_input) {
        my_game_view->grabKeyboard();
      }

      my_game_view->setMouseTracking(true);
      my_game_view->installEventFilter(this);
      my_game_view->setFocus();
      center_cursor(*my_game_view);


      SoundResourcePtr test_sound = application().core().resource_service().get_sound("level1_music");
      application().core().audio_service().play(test_sound, true);


      break;
  }

  my_mode = mode;
}

void EditorWindow::init_menu_view()
{
  my_ui->menu_view->addAction(my_ui->entity_list_dock->toggleViewAction());
  my_ui->menu_view->addAction(my_ui->entity_edit_dock->toggleViewAction());
  my_ui->menu_view->addAction(my_ui->undo_view_dock->toggleViewAction());
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
  switch_mode(Mode::EDIT);

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

  //QQQ send mouse event to the engine
  center_cursor(*my_game_view);
  return true;
}

void EditorWindow::on_action_run_triggered()
{
  log::debug(DEBUG_EDITOR, "Action run");

  if (my_mode == Mode::EDIT) {
    switch_mode(Mode::GAME);
  } else if (my_mode == Mode::GAME) {
    switch_mode(Mode::EDIT);
  } else {
    log::error("Unknown editor mode");
  }
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
  // erase old scene QQQ should be erased after successful load
  application().world()->clear();
  undo_stack().clear();

  // process selected file
  if (!load_from_file(filename, *application().world(), application().core().entity_creators(),
    application().core())) {
    QMessageBox::warning(this, "Error opening file", QString("Can't world file \"%1\"").arg(filename));
  }

  my_filename = filename;
}

}
}

#include "editor/ui/editor_window.moc"
