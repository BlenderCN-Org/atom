#include "player.h"
#include <core/world/entity.h>
#include <core/component/script_component.h>
#include <core/component/model_component.h>
#include <core/component/mesh_component.h>
#include <core/component/render_component.h>
#include <core/input/input_service.h>
#include <core/world/world.h>

namespace atom {

class PlayerScript : public ScriptComponent {
  Vec3f my_position;
  f32   my_camera_yaw;
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
  }
  
  void on_update() override
  {

    
    
    InputService &is = core().input_service();
    
    int dir = 0;
    
    if (is.is_key_down(Key::KEY_W)) {
      dir += 1;
    }
    
    if (is.is_key_down(Key::KEY_S)) {
      dir -= 1;
    }
    
    if (dir != 0) {
      Camera camera = world().camera();
    }
    
    Vec2f mouse = is.mouse().delta;
    
    my_camera_yaw += mouse.x;
    my_camera_pitch += mouse.y;
    
    BasicCamera camera;
    camera.set_position(my_position + Vec3f(0, 0, 2));
    camera.set_yaw(my_camera_yaw);
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
  
  uptr<Component> clone() const override
  {
    return uptr<Component>(new PlayerScript());
  }
  
public:
  META_DECLARE_CLASS;
  
  PlayerScript()
    : my_position(0, 0, 1)
    , my_camera_yaw(0)
    , my_camera_pitch(0)
  {
    META_INIT();
  }
};

META_DEFINE_FIELDS(PlayerScript) {
  FIELD(PlayerScript, my_position, "position"),
  FIELD(PlayerScript, my_camera_yaw, "camera_yaw"),
  FIELD(PlayerScript, my_camera_pitch, "camera_pitch")
};

META_DEFINE_CLASS(PlayerScript, ScriptComponent);

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
