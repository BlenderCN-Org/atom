//------------------------------------------------------------------------------
// Log system
//
// This file should be included in source files (not in header files).
//------------------------------------------------------------------------------

#pragma once

#include "config.h"

namespace atom {

void set_color_enabled(bool enabled);

/**
 * Vypise zadany text na stdout.
 */
void log_info(const char *format, ...);

/**
 * Vypise varovanie, ma podobne chovanie ako @see error, ale neukoncuje aplikaciu
 */
void log_warning(const char *format, ...);

/**
 * Vypise chybu a ukonci aplikaciu.
 */
void log_error(const char *format, ...);

/**
 * Debug vypis.
 */
#ifndef NDEBUG
void log_debug(bool print, const char *format, ...);
#else
inline void log_debug(bool print, const char *, ...) {}
#endif


#define not_tested() not_tested_message(ATOM_FUNC_NAME)
#define not_implemented() not_implemented_message(ATOM_FUNC_NAME)
#define subclass_responsibility() not_implemented_message(ATOM_FUNC_NAME)

void not_tested_message(const char *msg);
void not_implemented_message(const char *msg);
void subclass_responsibility_message(const char *msg);

#define DEBUG_VIDEO     Config::instance().debug_video
#define DEBUG_AUDIO     Config::instance().debug_audio
#define DEBUG_INPUT     Config::instance().debug_input
#define DEBUG_RESOURCES Config::instance().debug_resources
#define DEBUG_INOTIFY   Config::instance().debug_inotify
#define DEBUG_COUNTERS  Config::instance().debug_counters

}
