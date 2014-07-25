#pragma once

#include "../noncopyable.h"

namespace atom {

class AudioBuffer : private NonCopyable {
public:
  enum {
    CURRENT_POSITION = -1
  };

  virtual int read_old(void *buffer, int start, int size) = 0;
  virtual int get_sample_rate() = 0;
  virtual int get_channel_count() = 0;
  virtual int get_size() = 0;
//    virtual int get_sample_count() = 0;

  virtual ~AudioBuffer()
  {
  }
};

}
