#include "config.h"

#include <cstring>
#include <rapidjson/reader.h>
#include <rapidjson/document.h>
#include <rapidjson/filestream.h>
#include "constants.h"
#include "log.h"
#include "meta.h"
#include "utils.h"
#include "json_utils.h"

using namespace atom::utils;

namespace atom {

namespace {

void load_properties_from_json(const rapidjson::Value &root, const MetaClass &meta_class, void *data, bool ignore)
{
  MetaFieldsEnumarator prop(meta_class);
  const MetaField *p;

  while ((p = prop.next())) {
    ReadResult result = read_basic_property_from_json(root, *p, data);

    if (result == ReadResult::FAIL || (result == ReadResult::MISS && !ignore)) {
      log_warning("Can't read property \"%s\" for config", p->name);
    }
  }
}

}

META_CLASS(Config,
  FIELD(screen_width, "screen_width"),
  FIELD(screen_height, "screen_height"),
  FIELD(screen_bpp, "screen_bpp"),
  FIELD(color_log, "color_log"),
  FIELD(debug, "debug"),
  FIELD(debug_video, "debug_video"),
  FIELD(debug_audio, "debug_audio"),
  FIELD(debug_input, "debug_input"),
  FIELD(debug_resources, "debug_resources"),
  FIELD(debug_counters, "debug_counters")
)

void Config::set_screen_resolution(u32 width, u32 height)
{
  screen_width = width;
  screen_height = height;
}

Config::Config()
  : color_log(false)
  , debug_inotify(false)
  , debug_video(false)
  , debug_audio(false)
  , debug_input(false)
  , debug_resources(false)
  , debug_counters(false)
  , screen_width(1024)
  , screen_height(768)
  , screen_bpp(32)
{
  META_INIT();

  FILE *file = fopen(CONFIG_FILENAME, "r");

  if (file == nullptr) {
    log_warning("Can't open config");
  }

  rapidjson::FileStream input(file);
  rapidjson::Document doc;
  doc.ParseStream<0>(input);

  if (doc.HasParseError()) {
    log_warning("Parse error %s", doc.GetParseError());
  }
  fclose(file);

  load_properties_from_json(doc, meta_class(), this, true);

  log_info("Screen resolution %ix%ix%i", screen_width, screen_height, screen_bpp);

  load_environment();

  set_color_enabled(color_log);
}

void Config::load_environment()
{
  const char *name  = nullptr;
  const char *value = nullptr;

  name = "COLOR_LOG";
  value = getenv(name);

  if (value != nullptr) {
    color_log = !strcmp(value, "1");
  }
}

}
