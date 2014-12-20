#pragma once

#include <QString>
#include <QUndoCommand>
#include "editorfwd.h"

namespace atom {
namespace editor {

class EditorCommand : public QUndoCommand {
public:
  EditorCommand(const QString &text);
};

}
}
