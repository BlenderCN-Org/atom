#include "audio_service.h"
#include <SDL/SDL.h>
#include <algorithm>
#include "resources.h"
#include "sound.h"
#include "log.h"

namespace atom {

AudioService::AudioService()
  : my_next_id(0)
  , my_counter(0)
{
  // SDL_Init bolo zavolane uz v coreovi
  SDL_AudioSpec desired;
  SDL_AudioSpec obtained;
  desired.channels = AUDIO_CHANNELS;
  desired.format = AUDIO_S16;
  desired.freq = AUDIO_FREQUENCY;
  desired.padding = 0;
  desired.samples = AUDIO_SAMPLES;
  desired.callback = AudioService::mix;
  desired.userdata = this;

  if (SDL_OpenAudio(&desired, &obtained) != 0) {
    log::error("Can't initialize audio");
    return;
  }

  if (obtained.freq != AUDIO_FREQUENCY || obtained.channels != AUDIO_CHANNELS) {
    log::error("Your sound card doesn't support frequency 44100 stereo sound!");
    return;
  }

  // init buffer
  my_buffer.resize(obtained.size / AUDIO_SAMPLE_SIZE, 0);

  log::info("Audio initialized");
  SDL_PauseAudio(0);
}

AudioService::~AudioService()
{
}

void AudioService::mix_audio(u8 *buffer, u32 len)
{
  assert(len == my_buffer.size() * AUDIO_SAMPLE_SIZE);
  log::debug(DEBUG_AUDIO, "Audio buffer size %i bytes", len);

  if (my_sounds.empty()) {
    return;
  }

  memset(buffer, 0, len);

  i16 *dst = reinterpret_cast<i16 *>(buffer);

  u32 buffer_samples = my_buffer.size();
  u32 buffer_size = buffer_samples * AUDIO_SAMPLE_SIZE;

  std::lock_guard<std::mutex> lock(my_mutex);

  for (AudioTrack &stream : my_sounds) {
    if (stream.stopped) {
      continue;
    }

    u32 bytes = stream.sound->sound().read_old(&my_buffer[0], stream.offset, buffer_size);
    stream.offset += bytes;

    while (stream.repeat && bytes < buffer_size) {
      int ret = stream.sound->sound().read_old(&my_buffer[bytes / 2], 0, buffer_size - bytes);
      bytes += ret;
      stream.offset = ret;
    }

    int count = bytes / 2;

    for (int i = 0; i < count; ++i) {
      i32 a = my_buffer[i];
      i32 b = dst[i];

      if (a < 0 && b < 0) {
        dst[i] = (a + b) - (a * b) / I16_MIN;
      } else if (a > 0 && b > 0) {
        dst[i] = (a + b) - (a * b) / I16_MAX;
      } else {
        dst[i] = a + b;
      }
    }
  }

  // remove ended streams

//  mix_test_audio(buffer, len);

}

void AudioService::clear()
{
  std::lock_guard<std::mutex> lock(my_mutex);
  my_sounds.clear();
}

void AudioService::mix_test_audio(u8 *buffer, u32 len)
{
  log::info("Mixing audio %i", my_counter);
  u32 count = len / 2;
  i16 *samples = reinterpret_cast<i16 *>(buffer);

  for (u32 i = 0; i < count; i += 2) {
    float v = sin((my_counter + i) / 10.0f) * 1000;
    samples[i] = v;
    samples[i + 1] = v;
  }

  my_counter += count / 2;
}

void AudioService::mix(void *service, u8 *buffer, int len)
{
  assert(service != nullptr);
  assert(len > 0);

  reinterpret_cast<AudioService *>(service)->mix_audio(buffer, len);
}

AudioService::AudioTrack* AudioService::find_track(u32 id)
{
  std::lock_guard<std::mutex> lock(my_mutex);
  auto found = std::find_if(my_sounds.begin(), my_sounds.end(),
    [id](const AudioTrack &stream) { return stream.id == id; });

  return found != my_sounds.end() ? &(*found) : nullptr;
}

u32 AudioService::play(const SoundResourcePtr &sound, bool repeat)
{
  assert(sound != nullptr);
  AudioTrack stream;
  stream.id = my_next_id++;
  stream.repeat = repeat;
  stream.stopped = false;
  stream.offset = 0;
  stream.sound = sound;
  std::lock_guard<std::mutex> lock(my_mutex);
  my_sounds.push_back(stream);

  return stream.id;
}

void AudioService::stop(u32 id)
{
  std::lock_guard<std::mutex> lock(my_mutex);
  auto start = std::remove_if(my_sounds.begin(), my_sounds.end(),
    [id](AudioTrack &stream) { return stream.id == id; });

  my_sounds.erase(start, my_sounds.end());
}

}
