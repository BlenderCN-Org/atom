#pragma once

#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/types.h"
#include "core/slice.h"
#include "core/math/vec.h"
#include "core/video/video_buffer.h"
#include "core/video/stream_type.h"

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


//QQQ
//struct RawStream {
//  Type     type;
//  u32      id;
//  u32      count;
//  uptr<u8> data;
//};

}
