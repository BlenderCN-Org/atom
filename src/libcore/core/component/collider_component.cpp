#include "collider_component.h"
#include "../utils/bt_utils.h"

namespace atom {

//
// Collider component
//

void ColliderComponent::activate()
{

}

void ColliderComponent::deactivate()
{

}

ColliderComponent::ColliderComponent()
  : NullComponent(ComponentType::COLLIDER)
{

}

ColliderComponent::~ColliderComponent()
{

}

void ColliderComponent::set_collision_shape(uptr<btCollisionShape> &&shape)
{
  my_shape = std::move(shape);
}

//
// Static plane collider
//

uptr<Component> PlaneColliderComponent::clone() const
{
  return uptr<Component>(new PlaneColliderComponent(my_plane));
}

PlaneColliderComponent::PlaneColliderComponent(const Vec4f &plane)
  : my_plane(plane)
{
  btVector3 normal(plane.x, plane.y, plane.z);
  set_collision_shape(uptr<btCollisionShape>(new btStaticPlaneShape(normal, plane.w)));
}


//
// 3D Box collider
//

uptr<Component> BoxColliderComponent::clone() const
{
  return uptr<Component>(new BoxColliderComponent(my_size));
}

BoxColliderComponent::BoxColliderComponent(const Vec3f &extent)
  : my_size(extent)
{
  set_collision_shape(uptr<btCollisionShape>(new btBoxShape(to_bt_vector3(extent / 2.0f))));
}


}
