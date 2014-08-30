#pragma once

#include "../foundation.h"
#include "component.h"

class btCollisionShape;

namespace atom {

class ColliderComponent : public Component {
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
  Vec4f my_plane;   ///< plane equation (xyz normal, w constant)

  uptr<Component> clone() const override;
public:
  explicit PlaneColliderComponent(const Vec4f &plane);
};


/**
 * 3D Box collider.
 */
class BoxColliderComponent : public ColliderComponent {
  Vec3f my_size;

  uptr<Component> clone() const override;
public:
  explicit BoxColliderComponent(const Vec3f &extent);

};


/**
 * Sphere collider.
 */
class SphereColliderComponent : public ColliderComponent {
  f32 my_radius;

public:
  explicit SphereColliderComponent(f32 radius);

};


/**
 * Static mesh collider.
 */
class MeshColliderComponent : public ColliderComponent {

};

MAP_COMPONENT_TYPE(ColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(PlaneColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(BoxColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(SphereColliderComponent, COLLIDER)
MAP_COMPONENT_TYPE(MeshColliderComponent, COLLIDER)

}
