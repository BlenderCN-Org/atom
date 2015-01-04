#include "player.h"
#include <core/entity.h>
#include <core/script_component.h>
#include <core/model_component.h>
#include <core/mesh_component.h>
#include <core/render_component.h>
#include <core/material_component.h>
#include <core/debug_processor.h>
#include <core/input_service.h>
#include <core/world.h>
#include <core/geometry_processor.h>
#include "game.h"

namespace atom {

class PlayerScript : public ScriptComponent {
  Vec3f my_position;
  f32   my_yaw;
  f32   my_pitch;
  Vec3f my_normal;
  f32   my_velocity;

  Mat4f transform() const
  {
    return Mat4f::translation(my_position) * Mat4f::rotation_z(my_yaw);
  }

  void on_activate() override
  {
    BasicCamera camera;
    camera.set_position(my_position);
    camera.set_yaw(0);
    camera.set_pitch(0);
    my_pitch = -0.3;
    my_velocity = 0;
    my_normal = Vec3f::z_axis();

    Camera world_camera = world().camera();
    world_camera.view = camera.get_view_matrix();
    world().set_camera(world_camera);
    // correct player position
    collision_at(Vec2f(my_position.x, my_position.y), my_position);
    entity().set_transform(transform());
  }

  void on_update() override
  {
    InputService &is = core().input_service();

    Vec2f mouse = is.mouse().delta;

    my_pitch += mouse.y;

    f32 acoef = 0; // acceleration

    if (is.is_key_down(Key::KEY_W) || is.is_key_down(Key::KEY_UP)) {
      acoef = 1;
    }

    if (is.is_key_down(Key::KEY_S) || is.is_key_down(Key::KEY_DOWN)) {
      acoef = -5;
    }

    const f32 TURN_STEP = stf(1);

    if (is.is_key_down(Key::KEY_D) || is.is_key_down(Key::KEY_RIGHT)) {
      my_yaw -= TURN_STEP;
    }

    if (is.is_key_down(Key::KEY_A) || is.is_key_down(Key::KEY_LEFT)) {
      my_yaw += TURN_STEP;
    }

    f32 slope = std::acos(dot3(forward(), my_normal.normalized())) - PI2;
    log_info("Diff %f", slope);

    const f32 gravity = -9;
    const f32 rcoef = 0; //my_velocity > 5 ? pow(1.0002, my_velocity / 25) : 0;
    my_velocity = my_velocity + acoef + gravity * slope * my_velocity / 15 - rcoef;
    my_velocity = max(my_velocity, 0.0f);

    f32 speed = stf(my_velocity);


    // collision shape (triangle)
    const f32 step = speed;
    Vec3f v0(0, 0, step);
    Vec3f v1(0, step, 0);
    Vec3f v2(0, 0, -step);

    if (my_velocity > 0) {
      const Vec3f base = Vec3f::y_axis();
      Vec3f dir(0, speed, 0);
      Quatf rotation = Quatf::from_to_rotation(base, dir.normalized()).normalized();
      v0 = rotate(rotation, v0);
      v1 = rotate(rotation, v1);
      v2 = rotate(rotation, v2);
    }

    Mat4f t = transform();

    Ray r0(transform_point(t, v0), transform_vec(t, v1 - v0));
    Ray r1(transform_point(t, v1), transform_vec(t, v2 - v1));

    RayGeometryResult result0;
    RayGeometryResult result1;
    bool hit0 = processors().geometry.intersect_ray(r0, CollisionMask::WORLD | CollisionMask::ENEMY, result0);
    bool hit1 = processors().geometry.intersect_ray(r1, CollisionMask::WORLD | CollisionMask::ENEMY, result1);

    if (my_velocity > 0) {
      bool hit = hit0 || hit1;
      Vec3f normal;
      Vec3f pos;

      if (hit0 && result0.t <= 1.1) {
        normal = result0.normal;
        pos = result0.hit;
      } else if (hit1 && result1.t <= 1.1) {
        normal = result1.normal;
        pos = result1.hit;
      }

      if (hit) {
        my_position = pos;
        my_normal = normal;
      }
    }

    entity().set_transform(transform());
    refresh_camera();
  }

  Vec3f up() const
  {
    return Vec3f::z_axis();
  }

  Vec3f forward() const
  {
    return transform_vec(Mat4f::rotation_z(my_yaw), Vec3f::y_axis());
  }

  Vec3f right() const
  {
    return transform_vec(Mat4f::rotation_y(my_yaw), Vec3f::x_axis());
  }

  uptr<Component> clone() const override
  {
    return uptr<Component>(new PlayerScript());
  }

  bool collision_at(const Vec2f &pos, Vec3f &point)
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
    const Config &config = Config::instance();

    f32 fov = PI2;
    f32 screen_width = config.get_screen_width();
    f32 screen_height = config.get_screen_height();
    f32 aspect = screen_width / screen_height;


    Mat4f view_fix = Mat4f::rotation_x(PI2);
    Mat4f view_offset = Mat4f::translation(-2 * forward()) * Mat4f::translation(0, 0, 0.4);

    Mat4f view = view_offset * transform() * view_fix;

    processors().debug.draw_line(my_position, my_position + up(), Vec3f(0, 0, 1));
    processors().debug.draw_line(my_position, my_position + forward(), Vec3f(0, 1, 0));
    processors().debug.draw_line(my_position, my_position + right(), Vec3f(1, 0, 0));

    Camera world_camera = world().camera();
    world_camera.view = view.inverted();
    world_camera.projection = Mat4f::perspective(fov, aspect, 0.001f, 9999.0f);
    world().set_camera(world_camera);
  }

public:
  PlayerScript()
    : my_position(0, 0, 1)
  {
    META_INIT();
  }

  META_SUB_CLASS(ScriptComponent);
};

META_CLASS(PlayerScript,
 FIELD(my_position, "position"),
)

uptr<Entity> create_player(World &world, Core &core)
{
  uptr<Entity> entity(new Entity(world, core));
  uptr<ModelComponent> model(new ModelComponent());
  model->set_model_name("monkey_rider");
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
