#include "mesh_component.h"
#include "../system/resource_service.h"
#include "../system/core.h"

namespace atom {

META_DEFINE_FIELDS(MeshComponent) {
  FIELD(MeshComponent, my_mode, "mode")
};

META_DEFINE_CLASS(MeshComponent, Component, "MeshComponent");

void MeshComponent::activate()
{
  if (my_mode == MeshComponentMode::AUTO) {
    my_mesh = core().resource_service().get_mesh(my_model->get_model_name());
  }
}

void MeshComponent::deactivate()
{
  my_mesh.reset();
}

uptr<Component> MeshComponent::clone() const
{
  return uptr<MeshComponent>(new MeshComponent(my_mode));
}

MeshComponent::MeshComponent(MeshComponentMode mode)
  : NullComponent(ComponentType::MESH)
  , my_mode(mode)
  , my_model(this)
{
  META_INIT()
}

MeshComponent::~MeshComponent()
{
  // empty
}

MeshResourcePtr MeshComponent::mesh() const
{
  return my_mesh;
}

void MeshComponent::set_mesh(MeshResourcePtr mesh)
{
  my_mesh = mesh;
}

}
