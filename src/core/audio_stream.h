#pragma once

#include "constants.h"
#include "audio_buffer.h"

namespace atom {

const int AUDIO_BUFFER_COUNT = 3; ///< pocet OpenAL buffrov, pouzite v AudioStream
const int AUDIO_BUFFER_SIZE = 16384; ///< velkost OpenAL medzipamate, pouzite v AudioStream

class AudioStream {
public:
  // public methods
  AudioStream();
  ~AudioStream();

  void reset();
  void set_audio_buffer(AudioBuffer *buffer);
  void play();
  void pause();
  bool is_stream_finished();
  bool update();

private:
};

}
