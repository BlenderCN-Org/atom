#include "render_component.h"
#include "../system/resources.h"
#include "../video/mesh_tree_node.h"
#include "../world/world.h"
#include "../processor/render_processor.h"
#include "mesh_component.h"
#include "material_component.h"

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
  component->set_enabled(is_enabled());
  return std::move(component);
}

RenderComponent::RenderComponent()
  : Component(ComponentType::RENDER)
  , my_material(this, "")
  , my_mesh(this, "")
  , my_is_enabled(true)
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
