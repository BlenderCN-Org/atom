#include "render_component.h"
#include "core/system/resources.h"
#include "core/video/mesh_tree_node.h"
#include "core/world/world.h"
#include "core/processor/video_processor.h"

namespace atom {

RenderComponent::RenderComponent(Entity &entity, const MeshResourcePtr &mesh,
  const MaterialResourcePtr &material)
  : Component(entity)
  , my_mesh(mesh)
  , my_material(material)
  , my_node(std::make_shared<MeshTreeNode>())
{
  my_node->material = my_material;
  my_node->mesh_resource = my_mesh;
}

void RenderComponent::attach()
{
  MeshTree *mesh_tree = processors().video.mesh_tree();
  mesh_tree->add_node(my_node);
}

void RenderComponent::detach()
{
  MeshTree *mesh_tree = processors().video.mesh_tree();
  mesh_tree->remove_node(my_node);
}

void RenderComponent::update_transform()
{
  my_node->transformations = Mat4f::translation(entity().position());
}

}
