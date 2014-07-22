#include "render_component.h"
#include "core/system/resources.h"
#include "core/video/mesh_tree_node.h"
#include "core/world/world.h"
#include "core/processor/video_processor.h"
#include "core/component/mesh_component.h"
#include "core/component/material_component.h"

namespace atom {

void RenderComponent::activate()
{
  processors().video.register_component(this);
}

void RenderComponent::deactivate()
{
  processors().video.unregister_component(this);
}

uptr<Component> RenderComponent::clone() const
{
  uptr<RenderComponent> component(new RenderComponent());
  return std::move(component);
}

RenderComponent::RenderComponent()
  : Component(ComponentType::RENDER)
  , my_material(this, "")
  , my_mesh(this, "")
{
}

MaterialResourcePtr RenderComponent::material() const
{
  return my_material->material();
}

MeshResourcePtr RenderComponent::mesh() const
{
  return my_mesh->mesh();
}

}
