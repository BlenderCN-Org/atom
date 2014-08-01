#pragma once

#include "../foundation.h"
#include "video_buffer.h"

namespace atom {

enum class StreamId {
  VERTEX  = 1,
  NORMAL  = 10,
  UV      = 20,
  BINDEX  = 30,
  BWEIGHT = 40,
  INDEX   = 100
};


struct MeshStream {
  MeshStream(StreamId id, VideoBuffer &&data)
    : stream_id(id)
    , buffer(std::move(data))
  {
  }

  StreamId    stream_id;
  VideoBuffer buffer;
};

class Mesh : private NonCopyable {
public:     // public methods
  void add_stream(StreamId type, VideoBuffer &&buffer);

  const VideoBuffer* find_stream(StreamId stream_id) const;

  const VideoBuffer* get_uv() const;

  std::vector<uptr<MeshStream>> my_streams;
};

}
