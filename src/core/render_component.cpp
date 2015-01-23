#include "render_component.h"
#include "resources.h"
#include "mesh_tree_node.h"
#include "world.h"
#include "render_processor.h"
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
