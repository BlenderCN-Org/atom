#include "editor_application.h"

int main(int argc, char **argv)
{
  QCoreApplication::setOrganizationName("atom");
  QCoreApplication::setApplicationName("Editor");
  atom::editor::EditorApplication app(argc, argv);
  // initialize the engine (init video, audio, ..., load game lib, show main window)
  if (!app.init()) {
    return EXIT_FAILURE;
  }

  // load filename
  app.process_arguments();

  // run event loop
  return app.exec();
}
