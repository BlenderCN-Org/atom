#include "editor_command.h"

namespace atom {
namespace editor {

EditorCommand::EditorCommand(const QString &text)
  : QUndoCommand(text)
{
}

}
}
