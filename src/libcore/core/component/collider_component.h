#pragma once

#include "../foundation.h"
#include "component.h"

class btCollisionShape;

namespace atom {

class ColliderComponent : public NullComponent {
  uptr<btCollisionShape> my_shape;

  void activate() override;

  void deactivate() override;

protected:
  ColliderComponent();
  ~ColliderComponent();

  void set_collision_shape(uptr<btCollisionShape> &&shape);

public:
  btCollisionShape* get_collision_shape() const
  {
    return my_shape.get();
  }
};


/**
 * Static plane collider.
 */
class PlaneColliderComponent : public ColliderComponent {
  Vec3f my_normal;   ///< plane equation (xyz normal, w constant)
  f32   my_w;
  
  void activate();

  uptr<Component> clone() const override;
public:
  PlaneColliderComponent();
  
  void set_plane(const Vec3f &normal, f32 w);
  
  META_SUB_CLASS(ColliderComponent);
};


/**
 * 3D Box collider.
 */
class BoxColliderComponent : public ColliderComponent {
  Vec3f my_size;

  uptr<Component> clone() const override;
  
public:
  BoxColliderComponent();

  void set_size(const Vec3f &extent);
  
  META_SUB_CLASS(ColliderComponent);
};


/**
 * Static mesh collider.
 */
class MeshColliderComponent : public ColliderComponent {

};

MAP_COMPONENT_TYPE(ColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(PlaneColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(BoxColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(MeshColliderComponent, COLLIDER)

}
