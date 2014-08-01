#pragma once

#include "component.h"

namespace atom {

class SkeletonComponent : public Component {
  String my_mesh_name;
  std::vector<Mat4f> my_transforms;

  void activate() override;

  void deactivate() override;

  uptr<Component> clone() const override;

public:
  explicit SkeletonComponent(const String &mesh);

  Slice<Mat4f> get_transforms() const;
};

}
