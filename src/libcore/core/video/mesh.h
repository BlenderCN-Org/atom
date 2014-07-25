#pragma once

#include "../corefwd.h"
#include "../noncopyable.h"
#include "../types.h"
#include "../slice.h"
#include "../math/vec.h"
#include "video_buffer.h"
#include "stream_type.h"

namespace atom {

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
  enum MeshBufferFlags {
    EMPTY     = 0,
    VERTICES  = 1,
    TEXCOORDS = 4,
    INDICES   = 8
  };

  void add_stream(StreamId type, VideoBuffer &&buffer);

  const VideoBuffer* find_stream(StreamId stream_id) const;

  const VideoBuffer* get_uv() const;

  uint32_t    flags;
//  VideoBuffer texcoords;

  std::vector<uptr<MeshStream>> my_streams;
};

bool has_vertices(const Mesh &mesh_buffer);

bool has_texcoords(const Mesh &mesh_buffer);

bool has_indices(const Mesh &mesh_buffer);

}
