#include "core/audio/sound.h"

#include <vorbis/vorbisfile.h>
#include <string.h>
#include <fstream>
#include <memory>
#include "core/constants.h"
#include "core/utils/utils.h"
#include "core/log.h"

namespace atom {

const int LOAD_SOUND_BUFFER_SIZE = 8192; ///< velkost pomocneho buffru pri vytvarani Sound

uptr<Sound> Sound::create_from_file(
  const char *filename)
{
  assert(filename != nullptr);
  OggVorbis_File vorbis_file;

  int status = ov_fopen((char *) filename, &vorbis_file);

  if (status != 0) {
    utils::vorbis_open_error(status, filename);
    return nullptr;
  }

  int channel_count = vorbis_file.vi->channels;
  int sample_rate = vorbis_file.vi->rate;

  if (channel_count > 2) {
    error("Only 1 and 2 channel audio files are supported");
    return nullptr;
  }

  /// @todo otestovat ov_seekable, inak sa nealokuje spravna velkost
  /// todo 2otestovat ov_seekable, inak sa nealokuje spravna velkost

  //int total = ov_
  int offset = 0;
  int bytes;

  int pcm_total = ov_pcm_total(&vorbis_file, -1);
  int bytes_total = pcm_total * channel_count * 2; // 2 => 16bit per sample

  /// @todo unique_ptr
  uptr<u8[]> buffer(new u8[bytes_total]);

  while (offset < bytes_total) {
    bytes = ov_read(&vorbis_file, (char *) &buffer[offset], LOAD_SOUND_BUFFER_SIZE, 0, 2, 1, nullptr);

    if (bytes < 0) {
      log::info("Error while reading Sound data");
      break;
    }

    offset += bytes;
  }

  // zatvor datovy subor
  ov_clear(&vorbis_file);

  if (offset <= 0) {
    error("Can't create Sound from file \"%s\"", filename);
  }

  uptr<Sound> sound(new Sound());
  sound->my_sample_rate = sample_rate;
  sound->my_channel_count = channel_count;
  sound->my_samples_size = bytes_total;
  sound->my_samples = std::move(buffer);
  return sound;
}

Sound::Sound() :
    my_sample_rate(-1), my_channel_count(-1), my_position(0), my_samples_size(0), my_samples(nullptr)
{
}

Sound::~Sound()
{

}

int Sound::read_old(void *buffer, int start, int size)
{
  assert(buffer != nullptr);
  assert(start >= -1);

  if (start == AudioBuffer::CURRENT_POSITION) {
    start = my_position;
  } else if (start >= my_samples_size) {
//    log::error("Sound buffer overflow");
    return 0;
  }

  int end = std::min(start + size, my_samples_size);
  int bytes = end - start;

  memcpy(buffer, &my_samples[start], bytes);

  my_position += bytes;

  return bytes;
}

int Sound::get_sample_rate()
{
  return my_sample_rate;
}

int Sound::get_channel_count()
{
  return my_channel_count;
}

int Sound::get_size()
{
  return my_samples_size;
}

}
