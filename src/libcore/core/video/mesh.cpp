#include "mesh.h"

namespace atom {

void Mesh::add_stream(StreamId type, VideoBuffer &&buffer)
{
  uptr<MeshStream> stream(new MeshStream(type, std::move(buffer)));
  my_streams.push_back(std::move(stream));
}

const VideoBuffer* Mesh::find_stream(StreamId stream_id) const
{
  for (const uptr<MeshStream> &stream : my_streams) {
    if (stream->stream_id == stream_id) {
      return &stream->buffer;
    }
  }
  // nemam dany stream
  return nullptr;
}

const VideoBuffer *Mesh::get_uv() const
{
  return find_stream(StreamId::UV);
}

}
