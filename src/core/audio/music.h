#pragma once

#include <vorbis/vorbisfile.h>
#include "core/corefwd.h"
#include "core/ptr.h"
#include "core/types.h"
#include "core/constants.h"
#include "core/audio/audio_buffer.h"

namespace atom {

const int MUSIC_BUFFER_SIZE = 116384; ///< velkost OpenAL medzipamate v Music
const int MUSIC_BUFFER_COUNT = 3; ///< pocet OpenAL buffrov v Music

class Music : public AudioBuffer {
public:
    static uptr<Music> create_from_file(
      const char *filename);
    
    int read_old(void *buffer, int start, int size);
    int get_sample_rate();
    int get_channel_count();
    int get_size();

    ~Music();

private:
    Music();
    ///@todo presunut do cpp suboru
    static ov_callbacks our_callbacks;

    static size_t read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
    static int    seek_func(void *datasource, ogg_int64_t offset, int whence);
//    static int    close_func(void *datasource);
    static long   tell_func(void *datasource);

private:
    OggVorbis_File my_vorbis_file;
    int   my_position;      ///< position in uncompressed samples
    int   my_data_position; ///< position in compresed data
    int   my_data_size;     ///< velkost komprimovanych (zdrojovych) dat v bytoch
    int   my_samples_size;  ///< velkost dekodovanych dat v bytoch
    uptr<u8[]> my_data;  ///< complressed samples
};

}
