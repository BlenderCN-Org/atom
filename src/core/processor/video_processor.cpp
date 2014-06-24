#include "core/processor/video_processor.h"

#include <algorithm>
#include "core/system/resource_service.h"
#include "core/component/pixmap_component.h"

namespace atom {

VideoProcessor::VideoProcessor(VideoService &vs, ResourceService &rs)
  : my_vs(vs)
  , my_rs(rs)
  , my_gbuffer(vs)
{
}

VideoProcessor::~VideoProcessor()
{
  // empty
}

void VideoProcessor::set_resolution(int width, int height)
{
  my_gbuffer.set_resolution(width, height);
}

void VideoProcessor::add_pixmap(PixmapComponent *component)
{
  assert(component != nullptr);
  component->set_vs(&my_vs);
  sptr<MeshTreeNode> node(new MeshTreeNode());
  node->material = my_rs.get_material_resource("box");

  Vec3fArray vertices;
  vertices.push_back(Vec3f(-1, -1, 1));
  vertices.push_back(Vec3f( 1, -1, 1));
  vertices.push_back(Vec3f( 1,  1, 1));
  vertices.push_back(Vec3f(-1, -1, 1));
  vertices.push_back(Vec3f( 1,  1, 1));
  vertices.push_back(Vec3f(-1,  1, 1));

  Vec2fArray uv;
  uv.push_back(Vec2f(0, 0));
  uv.push_back(Vec2f(1, 0));
  uv.push_back(Vec2f(1, 1));
  uv.push_back(Vec2f(0, 0));
  uv.push_back(Vec2f(1, 1));
  uv.push_back(Vec2f(0, 1));

  sptr<Mesh> mesh(new Mesh());

  VideoBuffer vtx(my_vs);
  vtx.set_data(vertices);
  mesh->add_stream(StreamId::VERTEX, std::move(vtx));

  VideoBuffer texcoords(my_vs);
  texcoords.set_data(uv);
  mesh->add_stream(StreamId::UV, std::move(texcoords));
  node->mesh = mesh;

  String material_name = component->material();

  if (!material_name.empty()) {
    MaterialResourcePtr material = my_rs.get_material_resource(material_name);
    node->material = material;
  }

  component->set_node(node);
  my_pixmap_components.push_back(component);

  my_mesh_tree.add_node(node);
}

void VideoProcessor::remove_pixmap(PixmapComponent *component)
{
  assert(component != nullptr);

  auto found = std::find(my_pixmap_components.begin(),
    my_pixmap_components.end(), component);

  if (found == my_pixmap_components.end()) {
    log::warning("Can't remove PixmapComponent, the component wasn't found");
    return;
  }

  component->set_node(nullptr);
  my_mesh_tree.remove_node(component->node());
  my_pixmap_components.erase(found);
}

MeshTree* VideoProcessor::mesh_tree()
{
  return &my_mesh_tree;
}

void VideoProcessor::poll()
{
  for (PixmapComponent *component : my_pixmap_components) {
    Vec2f size = component->size();
//    log::info("Size is %s", to_string(size).c_str());
    if (component->is_auto_transform()) {
      Vec3f pos = component->entity().position();

      Mat4f t = Mat4f::translation(pos.x, pos.y, 0.0f) * Mat4f::scale(size.x, size.y, 1);

      if (component->is_inverted_horizontal()) {
          t = t * Mat4f::scale(-1, 1, 1);
      }
      component->node()->transformations = t;
    } else {
      component->node()->transformations = component->transform() * Mat4f::scale(size.x, size.y, 1);
    }
  }
}

GBuffer& VideoProcessor::get_gbuffer()
{
  return my_gbuffer;
}

}
