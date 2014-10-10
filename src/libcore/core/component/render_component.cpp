#include "render_component.h"
#include "../system/resources.h"
#include "../video/mesh_tree_node.h"
#include "../world/world.h"
#include "../processor/render_processor.h"
#include "mesh_component.h"
#include "material_component.h"

namespace atom {

META_CLASS(RenderComponent,
  FIELD(my_is_enabled, "enabled")             
)

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
  return uptr<Component>(new RenderComponent());
}

RenderComponent::RenderComponent()
  : NullComponent(ComponentType::RENDER)
  , my_material(this, "")
  , my_mesh(this, "")
  , my_is_enabled(true)
{
  META_INIT();
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
