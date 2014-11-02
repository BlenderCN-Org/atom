#include "player.h"
#include <core/world/entity.h>
#include <core/component/script_component.h>
#include <core/component/model_component.h>
#include <core/component/mesh_component.h>
#include <core/component/render_component.h>
#include <core/component/material_component.h>
#include <core/input/input_service.h>
#include <core/world/world.h>
#include <core/processor/geometry_processor.h>
#include "game.h"

namespace atom {

class PlayerScript : public ScriptComponent {
  Vec3f my_position;
  f32   my_camera_yaw;    ///< 0 is y_axis
  f32   my_camera_pitch;
  
  void on_activate() override
  {
    BasicCamera camera;
    camera.set_position(my_position);
    camera.set_yaw(my_camera_yaw);
    camera.set_pitch(my_camera_pitch);
    
    Camera world_camera = world().camera();
    world_camera.view = camera.get_view_matrix();
    world().set_camera(world_camera);
    // correct player position
    collision_at(Vec2f(my_position.x, my_position.y), my_position);
  }
  
  void on_update() override
  {
    InputService &is = core().input_service();
    
    Vec2f mouse = is.mouse().delta;
    
    my_camera_yaw -= mouse.x;
    my_camera_pitch += mouse.y;
    
    f32 step_length = is.is_key_down(Key::KEY_LSHIFT) ? 0.3f : 0.1f;
    
    f32 x_forward = -sin(my_camera_yaw);
    f32 y_forward =  cos(my_camera_yaw);
    
    f32 x_strafe = cos(my_camera_yaw);
    f32 y_strafe = sin(my_camera_yaw);
    
    Vec2f step_forward = Vec2f(x_forward, y_forward) * step_length;
    Vec2f step_strafe = Vec2f(x_strafe, y_strafe) * step_length;
    
    Vec2i dir(0, 0);
    
    if (is.is_key_down(Key::KEY_W)) {
      dir.y += 1;
    }
    
    if (is.is_key_down(Key::KEY_S)) {
      dir.y -= 1;
    }
    
    if (is.is_key_down(Key::KEY_D)) {
      dir.x += 1;
    }
    
    if (is.is_key_down(Key::KEY_A)) {
      dir.x -= 1;
    }
  
    Vec2f delta = dir.y * step_forward + dir.x * step_strafe;
    Vec2f pos = Vec2f(my_position.x, my_position.y) + delta;
    
    if (collision_at(pos, my_position)) {
      Mat4f transform = Mat4f::translation(my_position);
      entity().set_transform(transform);
    } else {
      my_position -= Vec3f(0.0f, 0.0f, 0.1f);
      Mat4f transform = Mat4f::translation(my_position);
      entity().set_transform(transform);
    }
    
    refresh_camera();
  }
  
  uptr<Component> clone() const override
  {
    return uptr<Component>(new PlayerScript());
  }
  
  bool collision_at(const Vec2f &pos, Vec3f &point) const
  {
    Ray ray(Vec3f(pos.x, pos.y, 100), -Vec3f::z_axis());
    RayGeometryResult result;
    bool hit = processors().geometry.intersect_ray(ray, CollisionMask::WORLD | CollisionMask::ENEMY, result);
    
    if (hit) {
      point = result.hit;
    }
    
    return hit;
  }
  
  void refresh_camera()
  {
    BasicCamera camera;
    camera.set_yaw(my_camera_yaw);
    camera.set_position(my_position + Vec3f(0, 0, 3) - camera.get_front() * 3);
    
    camera.set_pitch(my_camera_pitch);
    
    const Config &config = Config::instance();
    
    f32 fov = 1.57f;
    f32 screen_width = config.get_screen_width();
    f32 screen_height = config.get_screen_height();
    f32 aspect = screen_width / screen_height;
    
    Camera world_camera = world().camera();
    world_camera.view = camera.get_view_matrix();
    world_camera.projection = Mat4f::perspective(fov, aspect, 0.001f, 9999.0f);
    world().set_camera(world_camera);
  }
  
public:
  PlayerScript()
    : my_position(0, 0, 1)
    , my_camera_yaw(0)
    , my_camera_pitch(0)
  {
    META_INIT();
  }
  
  META_SUB_CLASS(ScriptComponent);
};

META_CLASS(PlayerScript,
 FIELD(my_position, "position"),
 FIELD(my_camera_yaw, "camera_yaw"),
 FIELD(my_camera_pitch, "camera_pitch")
)

uptr<Entity> create_player(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("player");
  entity->add_component(std::move(model));
  uptr<MaterialComponent> material(new MaterialComponent());
  material->set_material_name("player");
  entity->add_component(std::move(material));
  entity->add_component(uptr<Component>(new MeshComponent()));
  entity->add_component(uptr<Component>(new RenderComponent()));
  
  uptr<PlayerScript> script(new PlayerScript());
  entity->add_component(std::move(script));
  return entity;
}

}
