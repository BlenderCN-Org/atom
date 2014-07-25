#include "file_watch.h"

#ifdef __linux__
#include <unistd.h>
#endif

#include <algorithm>
#include <cassert>
#include <string.h>
#include "../log.h"
#include "../utils/utils.h"
#include "config.h"

#ifdef __linux__

namespace {

struct INotifyEventDescription {
  unsigned    code;
  const char *event;
  const char *description;
};

static INotifyEventDescription event_descriptions[] = {
  { 0x00000000, "UNKNOWN"         , "Unknown event."             },
  { 0x00000001, "IN_ACCESS"       , "File was accessed."         },
  { 0x00000002, "IN_MODIFY"       , "File was modified."         },
  { 0x00000004, "IN_ATTRIB"       , "Metadata changed."          },
  { 0x00000008, "IN_CLOSE_WRITE"  , "Writtable file was closed." },
  { 0x00000010, "IN_CLOSE_NOWRITE", "Unwrittable file closed."   },
  { 0x00000020, "IN_OPEN"         , "File was opened."           },
  { 0x00000040, "IN_MOVED_FROM"   , "File was moved from X."     },
  { 0x00000080, "IN_MOVED_TO"     , "File was moved to Y."       },
  { 0x00000100, "IN_CREATE"       , "Subfile was created."       },
  { 0x00000200, "IN_DELETE"       , "Subfile was deleted."       },
  { 0x00000400, "IN_DELETE_SELF"  , "Self was deleted."          },
  { 0x00000800, "IN_MOVE_SELF"    , "Self was moved."            },
  { 0x00002000, "IN_UNMOUNT"      , "Backing fs was unmounted."  },
  { 0x00004000, "IN_Q_OVERFLOW"   , "Event queued overflowed."   },
  { 0x00008000, "IN_IGNORED"      , "File was ignored."          }
};

const INotifyEventDescription& get_event_description(unsigned event)
{
  auto found = std::find_if(std::begin(event_descriptions), std::end(event_descriptions),
    [&event](const INotifyEventDescription &e) -> bool { return event == e.code; });

  if (found == std::end(event_descriptions))
    return event_descriptions[0];

  return *found;
}

}

namespace atom {

FileWatch::FileWatch()
{
  my_watch_fd = inotify_init();
  if (my_watch_fd < 0) {
    log::error("Can't initialize inotify \"%s\"", strerror(errno));
  }
}

FileWatch::~FileWatch()
{
  close(my_watch_fd);
}

void FileWatch::watch_dir(const char *dir, WatchMethod method)
{
  assert(dir != nullptr);
  assert(strlen(dir) > 0);

  WatchInfo info;
  info.wd = inotify_add_watch(my_watch_fd, dir, IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO);
  info.path = dir;
  info.is_dir = true;
  info.method = method;
  my_watch_info.push_back(info);
}

void FileWatch::watch_dir(
  const String &dir,
  WatchMethod method)
{
  watch_dir(dir.c_str(), method);
}

//void FileWatch::watch_file(const char *file)
//{
//  assert(file != nullptr);
//  assert(strlen(file) > 0);

//  WatchInfo info;
//  info.wd = inotify_add_watch(my_watch_fd, file, IN_CLOSE_WRITE);
//  info.path = file;
//  info.is_dir = false;
//  my_watch_info.push_back(info);
//}

void FileWatch::poll()
{
  assert(my_watch_fd >= 0);
  const unsigned BUFFER_SIZE = 1024;

  my_change_list.clear();
  // poll inotify
  pollfd pfd = { my_watch_fd, POLLIN, 0 };
  /// @todo pridat spracovanie inotify_event-u
  if (::poll(&pfd, 1, 0) > 0) {
    char buffer[1024];
    // precitaj inotify event, 1kB by mal stacit
    ssize_t bytes = read(my_watch_fd, buffer, BUFFER_SIZE);
    // spracuj nacitane eventy, vypis chbu ak sa nepodarilo event nacitat
    if (bytes > 0) {
      process_inotify_events(buffer, bytes);
//      log::info("Sizeof inotify_event is %i", sizeof(inotify_event));
//      refresh(changed_files);
    } else {
      log::error("Can't read inotify data - too small buffer");
    }
  }
}

ChangeList FileWatch::change_list()
{
  return my_change_list;
}

void FileWatch::process_inotify_events(const char *data, size_t len)
{
  assert(data != nullptr);
  const char *end = data + len;
  // postupne prejdi vsetky nacitane eventy
  while (data < end) {
    const inotify_event *event = reinterpret_cast<const inotify_event *>(data);
    data += sizeof(inotify_event) + event->len;
    log::debug(DEBUG_INOTIFY, "%s", to_string(event).c_str());

    const WatchInfo &info = get_watch_info(event->wd);
    const String name = get_full_path(event->wd, event->name);

    if (info.method == WatchMethod::CLOSE) {
      if (event->mask & IN_CLOSE_WRITE || event->mask & IN_MOVED_TO)
        my_change_list.push_back(name);
    } else if (info.method == WatchMethod::MODIFY_AND_CLOSE) {
      // spravi si zaznam o modifikacii
      if (event->mask & IN_MODIFY &&
          std::find(my_modifed_files.begin(), my_modifed_files.end(), name) == my_modifed_files.end()) {
        my_modifed_files.push_back(name);
      }

      // over ze subor bol najprv modifikovany a potom zatvoreny
      // v tomto pripade pridaj subor do change listu a odstran ho z modify listu
      if (event->mask & IN_CLOSE_WRITE) {
        auto found = std::find(my_modifed_files.begin(), my_modifed_files.end(), name);
        if (found != my_modifed_files.end()) {
          log::info("Changed %s", name.c_str());
          my_change_list.push_back(name);
          my_modifed_files.erase(found);
        }
      }
    } else if (info.method == WatchMethod::MOVE) {
      if (event->mask & IN_MOVED_TO &&
          std::find(my_modifed_files.begin(), my_modifed_files.end(), name) == my_modifed_files.end()) {
        my_change_list.push_back(name);
      }
    } else {
      error("Unknown WatchMethod %i", static_cast<int>(info.method));
    }
  }
}

String FileWatch::to_string(const inotify_event *event)
{
  assert(event != nullptr);
  // ziskaj textovy popis eventu
  const INotifyEventDescription &event_description = get_event_description(event->mask);
  // preved inotify event na retazec
  const uint BUFFER_SIZE = 1024;
  char buffer[BUFFER_SIZE];
  snprintf(buffer, BUFFER_SIZE, "%s %s %s (wd=%i, mask=%X, len=%i, name=%s)",
    event_description.event, event_description.description, get_full_path(event->wd,
    event->name).c_str(), event->wd, event->mask, event->len, event->name);
  return String(buffer);
}

String FileWatch::get_full_path(int wd, const char *name)
{
  auto found = std::find_if(my_watch_info.begin(), my_watch_info.end(),
    [&](const WatchInfo &info) { return info.wd == wd; });

  if (found != my_watch_info.end()) {
    if (found->is_dir)
      return found->path + "/" + name;
    else
      return String(name);
  } else {
    log::error("Can't find watch info for wd %i \"%s\"", wd, name);
    return String(name);
  }
}

void FileWatch::mark_file_as_modified(const char *name)
{
  auto found = std::find(my_modifed_files.begin(), my_modifed_files.end(), name);

  if (found == my_modifed_files.end())
    my_modifed_files.push_back(name);
}

bool FileWatch::is_file_modified(const char *name)
{
  return std::find(my_modifed_files.begin(), my_modifed_files.end(), name) != my_modifed_files.end();
}

FileWatch::WatchInfo& FileWatch::get_watch_info(int wd)
{
  auto found = std::find_if(my_watch_info.begin(), my_watch_info.end(),
    [&](const WatchInfo &info) -> bool { return info.wd == wd; });
  // pre kazdy wd musi existovat zaznam, inak je niekde chyba a dostavame eventy,
  // ktore nemame registrovane, preto assert
  assert(found != my_watch_info.end());
  return *found;
}

}

#else

namespace atom {

void FileWatch::watch_dir(const char *dir, WatchMethod method)
{
}

void FileWatch::watch_dir(const String &dir, WatchMethod method)
{
}

void FileWatch::poll()
{
}

ChangeList FileWatch::change_list()
{
  return ChangeList();
}

}

#endif
