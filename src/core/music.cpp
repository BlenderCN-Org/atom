#include "music.h"

#include <string.h>
#include <fstream>
#include "log.h"
#include "ptr.h"
#include "utils.h"

namespace atom {

ov_callbacks Music::our_callbacks = {
  read_func,
  seek_func,
  nullptr,
  tell_func,
};

size_t Music::read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
  assert(datasource != nullptr);
  assert(ptr != nullptr);

  Music *music = reinterpret_cast<Music *>(datasource);

  if (music->my_data_position >= music->my_data_size)
    return 0;

  int start = music->my_data_position;
  int end = min(music->my_data_size, (int)(start + size * nmemb));
  int bytes_total = end - start;

  memcpy(ptr, (u8 *)&music->my_data[0] + start, bytes_total);

  music->my_data_position += bytes_total;

  return bytes_total;
}

int Music::seek_func(void *datasource, ogg_int64_t offset, int whence)
{
  assert(datasource != nullptr);

  not_implemented();

  return -1;
}

long Music::tell_func(void *datasource)
{
  assert(datasource != nullptr);
  Music *music = (Music *)datasource;

  return music->my_data_position;
}

uptr<Music> Music::create_from_file(
  const char *filename)
{
  assert(filename != nullptr);

  if (filename == nullptr)
    return nullptr;

  std::ifstream input(filename, std::ios_base::in | std::ios_base::binary);

  if (!input.is_open()) {
    error("Can't open file \"%s\"\n", filename);
    return nullptr;
  }

  /// @todo unique_ptr
  long length = get_file_size(input);
  uptr<u8[]> data(new u8[length]);

  input.read((char *)&data[0], length);

  if (input.fail()) {
    error("Error while reading from file \"%s\"", filename);
    return nullptr;
  }

  if (input.peek() != EOF)
    log::warning("Invalid file size, reading less bytes than actual file size");

  input.close();

  uptr<Music> music(new Music());
  music->my_data      = std::move(data);
  music->my_data_size = length;

  int status = ov_open_callbacks(music.get(), &music->my_vorbis_file, nullptr, 0, our_callbacks);

  if (status != 0) {
    error("Can't open music file");
    return nullptr;
  }

  /// @todo zavolat a zistit status ov_seekable

  if (music->my_vorbis_file.vi->channels > 2) {
    error("Only 1-2 channel music is supported!!!");
    return nullptr;
  }

  // vypocitaj celkovu velkost dekomprinovanych dat
  int channel_count = music->my_vorbis_file.vi->channels;
  int sample_count  = ov_pcm_total(&music->my_vorbis_file, -1);
  music->my_samples_size = sample_count * channel_count * 2;

  return music;
}

Music::Music() : my_position(0), my_data_position(0), my_data_size(0),
                             my_samples_size(0), my_data(nullptr)
{
}

Music::~Music()
{
  ov_clear(&my_vorbis_file);
}

int Music::read_old(void *buffer, int start, int size)
{
  assert(buffer != nullptr);
  assert(start >= -1);

  if (start != -1 && start != my_position + 1) {
    // seek to desired position
    not_implemented();
  }

  assert(buffer != nullptr);
  assert(size > 0);

  int section;
  int bytes_read = 0;

  while (bytes_read < size) {
    int bytes = ov_read(&my_vorbis_file, (char *)buffer + bytes_read,
      size - bytes_read, 0, 2, 1, &section);

    if (bytes <= 0)
      break;

    bytes_read += bytes;
  }

  my_position += bytes_read;

  return bytes_read;
}

int Music::get_sample_rate()
{
  return my_vorbis_file.vi->rate;
}

int Music::get_channel_count()
{
  return my_vorbis_file.vi->channels;
}

int Music::get_size()
{
  return my_samples_size;
}

}
