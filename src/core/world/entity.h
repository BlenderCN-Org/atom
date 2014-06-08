#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/meta.h"
#include "core/string.h"
#include "core/noncopyable.h"
#include "core/math/mat.h"
#include "core/math/bounding_box.h"
#include "core/math/transformations.h"

namespace atom {

typedef sptr<World> WorldPtr;

/**
 * Entita herneho objektu.
 *
 * Notes:
 */
class Entity : NonCopyable {
public:

  Entity(World &world, Core &core, f32 width = 1, f32 height = 1);

  ~Entity();

  virtual uptr<Entity> clone(World &world) const = 0;

  /**
   * Entity was added to the world. Create graphic, physic objects... here.
   */
  void welcome();

  /**
   * Entity is going to be removed from the world. Delete graphic, physics objects, ... here.
   */
  void goodbye();

  void init();

  void update();

  virtual void on_welcome();
  virtual void on_goodbye();
  virtual void on_init();
  virtual void on_update();

  const String& id() const;

  void set_id(const String &id);

  Vec2f position() const;

  void set_position(const Vec2f position);

  void set_size(f32 width, f32 height);

  void update_bounding_box();

  const String& class_name() const;

  void set_class_name(const String &class_name);

  /// volat len ked je objekt zivy, je zaradeny do nejakeho sveta
  World &world() const;

  BoundingBox bounding_box() const;

  bool is_live() const;

  Core& core() const;

  enum class State {
    NEW,
    RUNNING
  };

  void register_component(Component *component);

public:
  META_DECLARE_CLASS_PTR; // each instance contains pointer to the MetaClass
  META_DECLARE_CLASS;     // static instance of MetaClass for Material

private:
  typedef std::vector<Component *> ComponentArray;

  void init(f32 width, f32 height, const Vec2f &position = Vec2f(0, 0), f32 rotation = 0);

private:
  World         &my_world;
  Core        &my_core;
  State          my_state;
  f32            my_width;
  f32            my_height;
  BoundingBox    my_bounding_box;
  String         my_id;
  String         my_class;
  Vec2f          my_position;
  f32            my_rotation;
  ComponentArray my_components;
};

}
