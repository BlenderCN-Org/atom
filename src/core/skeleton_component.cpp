#include "skeleton_component.h"
#include "model_component.h"
#include "entity.h"
#include "log.h"
#include "model.h"
#include "resource_service.h"
#include "resources.h"

namespace atom {

SkeletonComponent::SkeletonComponent()
  : NullComponent(ComponentType::SKELETON)
  , my_model(this)
{
  // empty
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

void SkeletonComponent::activate()
{
  ModelResourcePtr resource  = my_model->get_model();

  if (resource == nullptr) {
    log_warning("Can't find model for skeleton");
    return;
  }

  const Model &model = resource->model();

  i32 count = model.bones.size();
  log_info("Found %i bones", count);

  my_transforms.clear();
  my_transforms.resize(count, Mat4f());
  my_bones.clear();

  for (const DataBone &bone : model.bones) {
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

Mat4f SkeletonComponent::calculate_bone_matrix(const Bone &bone) const
{
  Mat4f m = Mat4f::translation(bone.local_head) * bone.transform.rotation_matrix() * Mat4f::translation(-bone.local_head);
  return bone.parent < 0 ? m : calculate_bone_matrix(my_bones[bone.parent]) * m;
}

}
