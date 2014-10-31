#include "geometry_processor.h"
#include "../component/geometry_component.h"
#include "../component/model_component.h"
#include "../component/skeleton_component.h"
#include "../system/resources.h"
#include "../math/intersect.h"
#include "../video/model.h"
#include "../utils/utils.h"

namespace atom {

void GeometryProcessor::regenerate_mesh(GeometryComponent &component)
{
//  component.
}

GeometryProcessor::GeometryProcessor(World &world)
  : NullProcessor(world)
{
  
}

GeometryProcessor::~GeometryProcessor()
{
  
}

void GeometryProcessor::poll()
{
  for (GeometryComponent *component : my_components) {
    // regenerate mesh
    if (component->is_dynamic()) {
      const Model *model = component->model();
      // skip components without geometry model data
      if (model == nullptr) {
        log::error("Dynamic GeometryComponent without model");
        continue;
      }
      // skip components without skeleton data
      const SkeletonComponent *skeleton = component->skeleton();
      if (skeleton == nullptr) {
        log::error("Dynamic GeometryComponent without skeleton");
        continue;
      }
      
      
      const ElementArray *vertices = model->find_array("vertices", Type::F32);
      const ElementArray *indices = model->find_array("indices", Type::U32);
      const ElementArray *bone_index = model->find_array("bone_index", Type::U32);
      const ElementArray *bone_weight = model->find_array("bone_weight", Type::F32);
  
      if (vertices == nullptr || indices == nullptr ||
          bone_index == nullptr || bone_weight == nullptr) {
        log::error("Dynamic GeometryComponent with invalid model");
        continue;
      }
      
      GeometryCache &cache = component->geometry_cache();
      cache.vertices.clear();
      
      const Vec3f *src_vertices = reinterpret_cast<const Vec3f *>(vertices->data.get());
      const u32 *src_bone_index = reinterpret_cast<const u32 *>(bone_index->data.get());
      const Vec4f *src_bone_weight = reinterpret_cast<const Vec4f *>(bone_weight->data.get());
      
      const Slice<Mat4f> transformations = skeleton->get_transforms();
      
      u32 count = vertices->size / sizeof(Vec3f);
      
      for (u32 i = 0; i < count; ++i) {
        u32 bi = src_bone_index[i];
        Vec4f weights = src_bone_weight[i];
        Vec3f v(0, 0, 0);
        
        for (u32 j = 0; j < 4; ++j, bi >>= 8) {
          u32 index = bi & 0xFF;
          f32 w = weights[j];
          v += transform_point(transformations[index], src_vertices[i]) * w;
        }
        cache.vertices.push_back(v);
      }
      
      log::info("Processing %i vertices", count);
    }
  }
}

bool GeometryProcessor::intersect_ray(const Ray &ray, u32 categories,
  RayGeometryResult &result)
{
  f32 tnearest = F32_MAX;
  u32 inearest = U32_MAX;
  Vec3f intersection;
  GeometryComponent *nearest = nullptr;
  
  for (GeometryComponent *component : my_components) {
    if ((component->categories() & categories) == 0) {
      continue;
    }
    
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
    Mat4f inverted = transform.inverted();
    Ray r(transform_point(inverted, ray.origin),
          transform_vec(inverted, ray.dir));
    
    u32 index;
    f32 t = intersect_mesh(r, v, i, index);

    if (t > 0 && t < tnearest) {
      inearest = index;
      tnearest = t;
      nearest = component;
      intersection = transform_point(transform, r.origin) + transform_vec(transform, r.dir) * t;
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
