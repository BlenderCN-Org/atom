#include "core/audio/audio_stream.h"

#include "core/log.h"
#include "core/types.h"

namespace atom {

AudioStream::AudioStream()
{
}

AudioStream::~AudioStream()
{
}

void AudioStream::reset()
{
}

void AudioStream::set_audio_buffer(AudioBuffer *buffer)
{
}

void AudioStream::play()
{
}

void AudioStream::pause()
{
}

bool AudioStream::is_stream_finished()
{
  return true;
}

bool AudioStream::update()
{
  return true;
}

}
