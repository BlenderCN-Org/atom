#include "mesh_component.h"
#include "model_component.h"
#include "resource_service.h"
#include "core.h"

namespace atom {

META_CLASS(MeshComponent,
  FIELD(my_mode, "mode")
)

void MeshComponent::activate()
{
  if (my_mode == MeshComponentMode::AUTO) {
    if (my_model->get_model() == nullptr) {
      log_error("%s: no model", ATOM_FUNC_NAME);
      return;
    }

    StringArray tokens = split_resource_name(my_model->get_model()->name());
    my_mesh = core().resource_service().get_mesh(tokens[1]);
  }
}

void MeshComponent::deactivate()
{
  my_mesh.reset();
}

MeshComponent::MeshComponent()
  : NullComponent(ComponentType::MESH)
  , my_mode(MeshComponentMode::AUTO)
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

void MeshComponent::set_mode(MeshComponentMode mode)
{
  my_mode = mode;
}

}
