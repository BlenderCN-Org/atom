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

  i32 count = mesh->raw_mesh().bones.size();
  log::info("Found %i bones", count);

  my_transforms.clear();
  my_transforms.resize(count, Mat4f::identity());

  for (const Bone &bone : mesh->raw_mesh().bones) {
    log::info("%s, parent %i", bone.name.c_str(), bone.parent);
  }
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

Slice<Mat4f> SkeletonComponent::get_transforms() const
{
  return Slice<Mat4f>(my_transforms.data(), my_transforms.size());
}

}
