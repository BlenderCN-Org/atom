#pragma once

#include <vector>
#include "processor.h"

namespace atom {

typedef std::vector<GeometryComponent *> GeometryComponentArray;

struct RayGeometryResult {
  GeometryComponent *component;
  Vec3f              hit;       ///< intersection
  f32                t;
  u32                triangle;  ///< triangle index
  Vec3f              normal;    ///< triangle normal
};

class GeometryProcessor : public NullProcessor {
  GeometryComponentArray my_components;
  
  void regenerate_mesh(GeometryComponent &component);
  
public:
  explicit GeometryProcessor(World &world);
  ~GeometryProcessor();
  
  void poll() override;
  
  bool intersect_ray(const Ray &ray, u32 categories, RayGeometryResult &result);
  
  void register_component(GeometryComponent *component);
  
  void unregister_component(GeometryComponent *component);
};

}
