#include "render_component.h"
#include "core/system/resources.h"
#include "core/video/mesh_tree_node.h"
#include "core/world/world.h"
#include "core/processor/video_processor.h"
#include "core/component/mesh_component.h"

namespace atom {

RenderComponent::RenderComponent(Entity &entity, MeshComponent &mesh,
  const MaterialResourcePtr &material)
  : Component(ComponentType::RENDER, entity)
  , my_mesh_component(mesh)
  , my_material(material)
{
}

void RenderComponent::attach()
{
  processors().video.register_component(this);
}

void RenderComponent::detach()
{
  processors().video.unregister_component(this);
}

const MaterialResourcePtr &RenderComponent::material() const
{
  return my_material;
}

const MeshResourcePtr& RenderComponent::mesh() const
{
  return my_mesh_component.mesh();
}

}
