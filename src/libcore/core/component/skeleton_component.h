#pragma once

#include "component.h"

namespace atom {

struct Bone {
  String name;
  Vec3f local_head;
  Vec3f x;
  Vec3f y;
  Vec3f z;
  i32 parent;
  Quatf transform;
};


class SkeletonComponent : public Component {
  String my_mesh_name;
  std::vector<Bone> my_bones;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

  Mat4f calculate_bone_matrix(const Bone &bone) const;

public:
  std::vector<Mat4f> my_transforms;

  explicit SkeletonComponent(const String &mesh);

  Slice<Mat4f> get_transforms() const;

  void recalculate_skeleton();

  Bone* find_bone(const String &name);
};

MAP_COMPONENT_TYPE(SkeletonComponent, SKELETON)

}
