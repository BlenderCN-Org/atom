#pragma once

#include "component.h"

namespace atom {

class SkeletonComponent : public Component {
  String my_mesh_name;


  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

public:
  std::vector<Mat4f> my_transforms;

  explicit SkeletonComponent(const String &mesh);

  Slice<Mat4f> get_transforms() const;
};

MAP_COMPONENT_TYPE(SkeletonComponent, SKELETON)

}
