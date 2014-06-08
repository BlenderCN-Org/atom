#pragma once

#include <core/log.h>

/**
 * Rozsirenie log-u z enginu o podporu debug vypisu pre editor.
 * Vsetok kod editoru by mal includovat tento subor namiesto <core/log.h>.
 */
namespace atom {
namespace editor {

const bool DEBUG_EDITOR = false;
const bool DEBUG_EDITOR_SAVE = false;
const bool DEBUG_EDITOR_LOAD = false;

}
}
