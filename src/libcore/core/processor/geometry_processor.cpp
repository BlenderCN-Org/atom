#include "geometry_processor.h"
#include "../component/geometry_component.h"
#include "../component/model_component.h"
#include "../system/resources.h"
#include "../math/intersect.h"
#include "../video/model.h"
#include "../utils/utils.h"

namespace atom {

GeometryProcessor::GeometryProcessor(World &world)
  : NullProcessor(world)
{
  
}

GeometryProcessor::~GeometryProcessor()
{
  
}

bool GeometryProcessor::intersect_ray(const Ray &ray, RayGeometryResult &result)
{
  f32 tnearest = F32_MAX;
  u32 inearest = U32_MAX;
  Vec3f intersection;
  GeometryComponent *nearest = nullptr;
  
  for (GeometryComponent *component : my_components) {
    Entity &entity = component->entity();
    Component *found = entity.find_component<ModelComponent>();
    // entity doesn't have model, skip it
    if (found == nullptr) {
      continue;
    }
    
    ModelComponent *model_component = static_cast<ModelComponent *>(found);
    ModelResourcePtr model_resource = model_component->get_model();
    
    if (model_resource == nullptr) {
      continue;
    }
    
    const Model &model = model_resource->model();
    
    const ElementArray *vertices = model.find_array("vertices", Type::F32);
    const ElementArray *indices = model.find_array("indices", Type::U32);

    if (vertices == nullptr || indices == nullptr) {
      continue;
    }

    const Slice<Vec3f> v(reinterpret_cast<const Vec3f *>(vertices->data.get()),
      vertices->size / sizeof(Vec3f));
    const Slice<u32> i(reinterpret_cast<const u32 *>(indices->data.get()),
                         indices->size / sizeof(u32));

    Mat4f transform = entity.transform();
    Ray r(transform * ray.origin, transform * ray.dir);

    u32 index;
    f32 t = intersect_mesh(r, v, i, index);

    if (t > 0 && t < tnearest) {
      inearest = index;
      tnearest = t;
      nearest = component;
      intersection = r.origin + r.dir * t;
    }
  }
  
  if (nearest != nullptr) {
    result.hit = intersection;
    result.component = nearest;
    result.triangle = inearest;
  }
  
  return nearest != nullptr;
}

void GeometryProcessor::register_component(GeometryComponent *component)
{
  assert(component != nullptr);
  my_components.push_back(component);
}

void GeometryProcessor::unregister_component(GeometryComponent *component)
{
  assert(component != nullptr);
  utils::erase_remove(my_components, component);
}

}
