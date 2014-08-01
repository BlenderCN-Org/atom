#pragma once

#include "../platform.h"
#include "../ptr.h"
#include "audio_buffer.h"

namespace atom {

class Sound : public AudioBuffer {
public: // public methods
  static uptr<Sound> create_from_file(
    const char *filename);

  ~Sound();

  int read_old(void *buffer, int pos, int size);
  int get_sample_rate();
  int get_channel_count();
  int get_size();

private:  // private methods
  Sound();

private:  // private members
  int                     my_sample_rate; ///< vzorkovacia frekvencia
  int                     my_channel_count; ///< pocet kanalov
  int                     my_position; ///< pozicia za posledne precitanym bytom, na pociatku 0
  int                     my_samples_size; ///< velkost dat (samplov) v bytoch
  uptr<u8[]> my_samples; ///< dekodovane audio data (samples)
};

}
