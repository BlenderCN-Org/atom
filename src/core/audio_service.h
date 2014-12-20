#pragma once

#include <deque>
#include <mutex>
#include "music.h"
#include "audio_stream.h"
#include "foundation.h"
#include "singleton.h"
#include "core.h"

namespace atom {

class AudioService {
public:
  enum {
    INVALID_ID = 0xFFFFFFFF
  };

  // public methods
  AudioService();
  ~AudioService();

  u32 play(const SoundResourcePtr &sound, bool repeat = false);

  void stop(u32 id);

  void clear(); // stop

  // void pause();

private:
  struct AudioTrack {
    u32              id;
    bool             stopped;
    bool             repeat;
    u64              offset;
    SoundResourcePtr sound;
  };

  // private definitions
  typedef std::deque<AudioStream *> AudioStreamList;

  void mix_audio(u8 *buffer, u32 len);

  void mix_test_audio(u8 *buffer, u32 len);

  static void mix(void *service, u8 *buffer, int len);

  AudioTrack* find_track(u32 id);

private:
  u32                    my_next_id;
  u32                    my_counter;
  std::deque<AudioTrack> my_sounds;
  std::vector<i16>       my_buffer;  ///< sample buffer
  std::mutex             my_mutex;

private:
  friend class Core;
};

}
