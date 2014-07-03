#include "mesh_component.h"
#include "core/system/resource_service.h"

namespace atom {

MeshComponent::MeshComponent(Entity &entity, const String &mesh)
  : Component(ComponentType::MESH, entity)
  , my_mesh_name(mesh)
{
  // empty
}

MeshComponent::~MeshComponent()
{
  // empty
}

void MeshComponent::attach()
{
  my_mesh = core().resource_service().get_mesh(my_mesh_name);
}

void MeshComponent::detach()
{
  my_mesh.reset();
}

const MeshResourcePtr& MeshComponent::mesh() const
{
  return my_mesh;
}

}