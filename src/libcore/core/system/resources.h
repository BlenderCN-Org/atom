#pragma once

#include "../corefwd.h"
#include "../noncopyable.h"
#include "../ptr.h"
#include "../string.h"
#include "../stdvec.h"

namespace atom {

typedef sptr<Resource> ResourcePtr;

String make_resource_name(const String &tag, const String &name);

class Loader;

class Resource : public NonCopyable {
public:
  Resource();

  virtual ~Resource()
  {}

  void depend_on_resource(const ResourcePtr &resource);

  void depend_on_file(const char *filename);

  void depend_on_file(const String &filename);

  void depend_on_files(const StringArray &files);

  const String& name() const
  {
    return my_name;
  }

  void set_name(const String &name)
  {
    my_name = name;
  }

  const StringArray& sources() const
  {
    return my_sources;
  }

  Loader* loader() const
  {
    return my_loader;
  }

  void set_loader(Loader *loader);

private:
  Loader *my_loader;
  String my_name;
  StringArray my_sources;
};

template<typename T>
class StandardResource : public Resource {
public:
  T* data()
  {
    return my_data.get();
  }

  const T* data() const
  {
    return my_data.get();
  }

  void set_data(uptr<T> &&data)
  {
    my_data = std::move(data);
  }

  void set_data(T *data)
  {
    my_data.reset(data);
  }

private:
  uptr<T> my_data;
};

StringArray split_resource_name(const String &resource_name);

//-----------------------------------------------------------------------------
//
// Image Resource
//
//-----------------------------------------------------------------------------

class ImageResource : public StandardResource<Image> {
public:
  ~ImageResource();

  const Image& image() const
  {
    return *data();
  }

private:
  String my_filename;
};

//-----------------------------------------------------------------------------
//
// Texture Resource
//
//-----------------------------------------------------------------------------

class TextureResource : public StandardResource<Texture> {
public:
  ~TextureResource();

  const Texture& texture() const
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// Shader Resource
//
//-----------------------------------------------------------------------------

class TechniqueResource : public StandardResource<Technique> {
public:
  ~TechniqueResource();

  Technique& program()
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// Material Resource
//
//-----------------------------------------------------------------------------

class MaterialResource : public StandardResource<Material> {
public:
  ~MaterialResource();

  Material& material()
  {
    return *data();
  }
};


//-----------------------------------------------------------------------------
//
// Bitmap Font Resource
//
//-----------------------------------------------------------------------------

class BitmapFontResource : public StandardResource<BitmapFont> {
public:
  ~BitmapFontResource();

  const BitmapFont& bitmap_font() const
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// RawMesh Resource
//
//-----------------------------------------------------------------------------

class RawMeshResource : public StandardResource<RawMesh> {
public:
  ~RawMeshResource();

  const RawMesh& raw_mesh() const
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// Mesh Resource
//
//-----------------------------------------------------------------------------

class MeshResource : public StandardResource<Mesh> {
public:
  ~MeshResource();

  const Mesh& mesh() const
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// Sound Resource
//
//-----------------------------------------------------------------------------

class SoundResource : public StandardResource<Sound> {
public:
  ~SoundResource();

  Sound& sound()
  {
    return *data();
  }
};

//-----------------------------------------------------------------------------
//
// Music Resource
//
//-----------------------------------------------------------------------------

class MusicResource : public StandardResource<Music> {
public:
  ~MusicResource();

  const Music& music() const
  {
    return *data();
  }
};

template<>
inline Type type_of<TechniqueResourcePtr>()
{
  return Type::SHADER;
}

template<>
inline Type type_of<TextureResourcePtr>()
{
  return Type::TEXTURE;
}

}
