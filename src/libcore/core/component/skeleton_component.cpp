#include "skeleton_component.h"
#include "../world/entity.h"
#include "../log.h"
#include "../video/raw_mesh.h"
#include "../system/resource_service.h"

namespace atom {

void SkeletonComponent::activate()
{
  RawMeshResourcePtr mesh = core().resource_service().get_raw_mesh(my_mesh_name);

  if (mesh == nullptr) {
    log::warning("Can't find mesh for skeleton");
    return;
  }

  log::info("Found %i bones", mesh->raw_mesh().bones.size());
}

void SkeletonComponent::deactivate()
{

}

uptr<Component> SkeletonComponent::clone() const
{
  return uptr<Component>(new SkeletonComponent(my_mesh_name));
}

SkeletonComponent::SkeletonComponent(const String &mesh)
  : Component(ComponentType::SKELETON)
  , my_mesh_name(mesh)
{
  assert(!mesh.empty() && "Mesh name must be provided");
}

}
