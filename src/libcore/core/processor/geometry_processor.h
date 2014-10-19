#pragma once

#include <vector>
#include "processor.h"

namespace atom {

typedef std::vector<GeometryComponent *> GeometryComponentArray;

struct RayGeometryResult {
  GeometryComponent *component;
  Vec3f              hit;
  u32                triangle;
};

class GeometryProcessor : public NullProcessor {
  GeometryComponentArray my_components;
  
  void regenerate_mesh(GeometryComponent &component);
  
public:
  explicit GeometryProcessor(World &world);
  ~GeometryProcessor();
  
  void poll() override;
  
  bool intersect_ray(const Ray &ray, RayGeometryResult &result);
  
  void register_component(GeometryComponent *component);
  
  void unregister_component(GeometryComponent *component);
};

}
