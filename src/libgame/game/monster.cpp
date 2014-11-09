#include "monster.h"
#include <core/world/world.h>
#include <core/component/skeleton_component.h>

namespace atom {

MonsterScript::MonsterScript()
  : my_skeleton(this)
{
}

void MonsterScript::on_update()
{
  f32 t = (sin(world().time()) + 1) / 2;  // t = <0, 1>
  f32 t1 = sin(world().time());

//  entity().set_transform(Mat4f::rotation_z(t));

  f32 tibia= t / 3;
  f32 radius = t / 3;

//  my_skeleton->find_bone("head")->transform = Quatf::from_axis_angle(Vec3f::z_axis(), t1 / 3);
//  my_skeleton->find_bone("tibia.R")->transform = Quatf::from_axis_angle(Vec3f::y_axis(), tibia);
//  my_skeleton->find_bone("feet.R")->transform = Quatf::from_axis_angle(Vec3f::y_axis(), tibia);
//  my_skeleton->find_bone("radius.R")->transform = Quatf::from_axis_angle(Vec3f::y_axis(), radius);
  my_skeleton->recalculate_skeleton();
}

uptr<Component> MonsterScript::clone() const
{
  return uptr<Component>(new MonsterScript());
}

}
