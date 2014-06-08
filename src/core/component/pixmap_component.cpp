#include "pixmap_component.h"
#include "core/world/world.h"
#include "core/processor/video_processor.h"
#include "core/system/resource_service.h"

namespace atom {

PixmapComponent::PixmapComponent(Entity &entity)
  : Component(entity)
  , my_vs(nullptr)
  , my_is_visible(true)
  , my_auto_transform(true)
  , my_transform(Mat4f::identity())
  , my_size(1, 1)
  , my_uv_start(0, 0)
  , my_uv_end(1, 1)
  , my_invert_horizontal(false)
{

}

void PixmapComponent::on_attach()
{
  world().processors().video.add_pixmap(this);
}

void PixmapComponent::on_detach()
{
  world().processors().video.remove_pixmap(this);
}

const String& PixmapComponent::material() const
{
  return my_material;
}

void PixmapComponent::set_model(const String &model)
{
  MeshResourcePtr resource = core().resource_service().get_mesh_resource(model);

  if (resource != nullptr) {
    log::info("Mesh %s successfully loaded", model.c_str());
    my_node->mesh_resource = resource;
  } else {
    log::warning("Can't load mesh %s", model.c_str());
  }
}

void PixmapComponent::set_material(const String &material)
{
  my_material = material;
}

Vec2f PixmapComponent::size() const
{
  return my_size;
}

void PixmapComponent::set_size(const Vec2f size)
{
  my_size = size;
}

void PixmapComponent::set_uv(const Vec2f &start, const Vec2f &end)
{
  assert(my_vs != nullptr);

  my_uv_start = start;
  my_uv_end = end;

  Vec2fArray uv;
  uv.push_back(Vec2f(start.x, start.y));
  uv.push_back(Vec2f(end.x, start.y));
  uv.push_back(Vec2f(end.x, end.y));
  uv.push_back(Vec2f(start.x, start.y));
  uv.push_back(Vec2f(end.x, end.y));
  uv.push_back(Vec2f(start.x, end.y));

  VideoBuffer texcoords(*my_vs);
  texcoords.set_data(uv);
  my_node->mesh->add_stream(StreamId::UV, std::move(texcoords));
}

bool PixmapComponent::is_auto_transform() const
{
  return my_auto_transform;
}

void PixmapComponent::set_auto_transform(bool enable)
{
  my_auto_transform = enable;
}

const Mat4f& PixmapComponent::transform() const
{
  return my_transform;
}

void PixmapComponent::set_transform(const Vec2f &position)
{
  my_transform = Mat4f::translation(position.x, position.y, 0);
}

bool PixmapComponent::visible() const
{
  return my_is_visible;
}

void PixmapComponent::set_visible(bool visible)
{
  my_is_visible = visible;

  if (my_node != nullptr) {
    my_node->visible = visible;
  }
}

sptr<MeshTreeNode> PixmapComponent::node() const
{
  return my_node;
}

void PixmapComponent::set_node(const sptr<MeshTreeNode> &node)
{
  my_node = node;
}

void PixmapComponent::set_vs(VideoService *vs)
{
  my_vs = vs;
}

bool PixmapComponent::is_inverted_horizontal() const
{
  return my_invert_horizontal;
}

void PixmapComponent::set_invert_horizontal(bool invert)
{
  my_invert_horizontal = invert;
}

}
