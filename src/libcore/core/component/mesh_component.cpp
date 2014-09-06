#include "mesh_component.h"
#include "../system/resource_service.h"
#include "../system/core.h"

namespace atom {

void MeshComponent::activate()
{
  my_mesh = core().resource_service().get_mesh(my_model->get_model_name());
}

void MeshComponent::deactivate()
{
  my_mesh.reset();
}

uptr<Component> MeshComponent::clone() const
{
  return uptr<MeshComponent>(new MeshComponent());
}

MeshComponent::MeshComponent()
  : Component(ComponentType::MESH)
  , my_model(this)
{
  // empty
}

MeshComponent::~MeshComponent()
{
  // empty
}




MeshResourcePtr MeshComponent::mesh() const
{
  return my_mesh;
}

}
