#include "mesh_component.h"
#include "core/system/resource_service.h"

namespace atom {

MeshComponent::MeshComponent(const String &mesh)
  : Component(ComponentType::MESH)
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

uptr<Component> MeshComponent::clone() const
{
  uptr<MeshComponent> component(new MeshComponent(my_mesh_name));
  return std::move(component);
}

MeshResourcePtr MeshComponent::mesh() const
{
  return my_mesh;
}

}
