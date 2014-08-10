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
  my_bones.clear();

  for (const DataBone &bone : mesh->raw_mesh().bones) {
    log::info("%s, parent %i", bone.name.c_str(), bone.parent);
    Bone b;
    b.name = bone.name;
    b.local_head = bone.local_head;
    b.x = bone.x;
    b.y = bone.y;
    b.z = bone.z;
    b.transform = Quatf();
    b.parent = bone.parent;
    my_bones.push_back(b);
  }
}

void SkeletonComponent::deactivate()
{

}

uptr<Component> SkeletonComponent::clone() const
{
  return uptr<Component>(new SkeletonComponent(my_mesh_name));
}

Mat4f SkeletonComponent::calculate_bone_matrix(const Bone &bone) const
{
  Mat4f m = Mat4f::translation(bone.local_head) * bone.transform.rotation_matrix() * Mat4f::translation(-bone.local_head);
//  Mat4f m = Mat4f::translation(bone.local_head) * Mat4f::rotation_x(bone.angle) * Mat4f::translation(-bone.local_head);
  return bone.parent < 0 ? m : calculate_bone_matrix(my_bones[bone.parent]) * m;
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

void SkeletonComponent::recalculate_skeleton()
{
  int i = 0;
  for (Bone &bone : my_bones) {
    my_transforms[i] = calculate_bone_matrix(bone);
    ++i;
  }
}

Bone* SkeletonComponent::find_bone(const String &name)
{
  for (Bone &bone : my_bones) {
    if (bone.name == name) {
      return &bone;
    }
  }

  return nullptr;
}

}
