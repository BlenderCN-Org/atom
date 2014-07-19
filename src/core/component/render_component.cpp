#include "render_component.h"
#include "core/system/resources.h"
#include "core/video/mesh_tree_node.h"
#include "core/world/world.h"
#include "core/processor/video_processor.h"
#include "core/component/mesh_component.h"

namespace atom {

RenderComponent::RenderComponent(MeshComponent &mesh,
  const MaterialResourcePtr &material)
  : Component(ComponentType::RENDER)
  , my_mesh_component(mesh)
  , my_material(material)
{
  assert(my_material != nullptr && "Material can't be nullptr");
}

void RenderComponent::attach()
{
  processors().video.register_component(this);
}

void RenderComponent::detach()
{
  processors().video.unregister_component(this);
}

uptr<Component> RenderComponent::clone() const
{
  uptr<RenderComponent> component(new RenderComponent(my_mesh_component, my_material));
  return std::move(component);
}

const MaterialResourcePtr& RenderComponent::material() const
{
  return my_material;
}

MeshResourcePtr RenderComponent::mesh() const
{
  assert(my_mesh_component.mesh() != nullptr);
  return my_mesh_component.mesh();
}

}
