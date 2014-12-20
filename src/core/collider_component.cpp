#include "collider_component.h"
#include "bt_utils.h"

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

META_CLASS(PlaneColliderComponent,
  FIELD(my_normal, "normal"),
  FIELD(my_w, "w")
)

void PlaneColliderComponent::activate()
{
  set_collision_shape(uptr<btCollisionShape>(
    new btStaticPlaneShape(to_bt_vector3(my_normal), my_w)));
}

uptr<Component> PlaneColliderComponent::clone() const
{
  return uptr<Component>(new PlaneColliderComponent());
}

PlaneColliderComponent::PlaneColliderComponent()
{
  META_INIT();
}

void PlaneColliderComponent::set_plane(const Vec3f &normal, f32 w)
{
  my_normal = normal;
  my_w = w;
}


//
// 3D Box collider
//

META_CLASS(BoxColliderComponent,
  FIELD(my_size, "size")
)

uptr<Component> BoxColliderComponent::clone() const
{
  return uptr<Component>(new BoxColliderComponent());
}

BoxColliderComponent::BoxColliderComponent()
  : my_size(1, 1, 1)
{
  META_INIT();
  set_collision_shape(uptr<btCollisionShape>(new btBoxShape(to_bt_vector3(my_size / 2.0f))));
}

void BoxColliderComponent::set_size(const Vec3f &size)
{
  assert(size.x > 0);
  assert(size.y > 0);
  assert(size.z > 0);
  my_size = size;
  set_collision_shape(uptr<btCollisionShape>(new btBoxShape(to_bt_vector3(my_size / 2.0f))));
}

}
