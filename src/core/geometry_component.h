#pragma once

#include "component.h"

namespace atom {

struct GeometryCache {
  std::vector<Vec3f> vertices;
};

class GeometryComponent : public NullComponent {
  bool                    my_is_dynamic;
  u32                     my_categories;
  Slot<ModelComponent>    my_model;
  Slot<SkeletonComponent> my_skeleton;
  GeometryCache           my_cache;
  
  void init() override;
  void terminate() override;
  
  uptr<Component> clone() const override;
  
public:
  GeometryComponent();
  
  void set_dynamic(bool dynamic)
  {
    my_is_dynamic = dynamic;
  }
  
  bool is_dynamic() const
  {
    return my_is_dynamic;
  }
  
  u32 categories() const
  {
    return my_categories;
  }
  
  void set_categories(u32 mask);
  
  const Model* model() const;
  
  const SkeletonComponent* skeleton() const;
  
  GeometryCache& geometry_cache()
  {
    return my_cache;
  }
  
  META_SUB_CLASS(NullComponent);
};

MAP_COMPONENT_TYPE(GeometryComponent, GEOMETRY)

}
