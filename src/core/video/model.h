#pragma once

#include "core/stdvec.h"
#include "core/video/stream_type.h"
#include "core/noncopyable.h"

namespace atom {

struct Stream {
  StreamId  type;
  ByteArray data;
};


struct Model {
public:

  void add_stream(StreamId id, ByteArray &&data);

  std::vector<uptr<Stream>> streams;
};

}
