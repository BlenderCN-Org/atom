#include "model.h"

namespace atom {

void Model::add_stream(StreamId id, ByteArray &&data)
{
  uptr<Stream> stream(new Stream());
  stream->type = id;
  stream->data = std::move(data);
  streams.push_back(std::move(stream));
}

}
