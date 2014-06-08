#pragma once

#ifdef __linux__
#include <sys/inotify.h>
#include <poll.h>
#endif

#include <vector>
#include "core/noncopyable.h"
#include "core/string.h"

namespace atom {

typedef std::vector<String> ChangeList;

enum class WatchMethod {
  CLOSE,             ///< zatvorenie suboru IN_CLOSE_WRITE
  MODIFY_AND_CLOSE,  ///< pred zatvorenim musi prist modifikacia, vhodne napr. pre shared kniznice
  MOVE
};

/**
 * Potrebujete zistit ked sa subor zmeni? Ste na spravnom mieste. FileWatch je trieda vyuzivajuca
 * mechanizmus inotify na sledovanie zmien suborov.
 *
 * Pouzitie:
 *   FileWatch watch;
 *   ...
 *   periodicky volat watch.poll(); a za nim watch.change_list(); obsahuje zoznam zmenenych suborov
 *   ...
 */

#ifdef __linux__
class FileWatch : private NonCopyable {
public:
  FileWatch();
  ~FileWatch();

  void watch_dir(const char *dir, WatchMethod method = WatchMethod::CLOSE);
  void watch_dir(const String &dir, WatchMethod method = WatchMethod::CLOSE);
//  void watch_file(const char *file);

  void poll();

  ChangeList change_list();

private:
  struct WatchInfo {
    int         wd;     ///< inotify watch directory descriptor
    bool        is_dir;
    String      path;
    WatchMethod method;
  };

  typedef std::vector<WatchInfo> WatchInfoVector;

private:
  /**
   * Spracuje inotify change list.
   *
   * @param data ukazatel na inotify data (data precitane funkciou read)
   * @param len dlzka inotify dat
   */
  void process_inotify_events(const char *data, size_t len);

  String to_string(const inotify_event *event);

  String get_full_path(int wd, const char *name);

  void mark_file_as_modified(const char *name);

  bool is_file_modified(const char *name);

  WatchInfo& get_watch_info(int wd);

private:
  int             my_watch_fd;             ///< inotify file descriptor
  WatchInfoVector my_watch_info;  ///< zoznam sledovanych suborov/adresarov
  ChangeList      my_change_list;
  ChangeList      my_modifed_files;
};

#else

class FileWatch : private NonCopyable {
public:
  void watch_dir(const char *dir, WatchMethod method = WatchMethod::CLOSE);
  void watch_dir(const String &dir, WatchMethod method = WatchMethod::CLOSE);

  void poll();

  ChangeList change_list();
};

#endif

}
