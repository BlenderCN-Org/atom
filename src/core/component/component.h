#pragma once

#include <cassert>
#include <vector>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/meta.h"
#include "core/string.h"

namespace atom {

enum class ComponentType {
  UNKNOWN,
  RENDER,
  MESH,
  RAW_MESH,
  SCRIPT,
  SKELETON
};

class Component : NonCopyable {
  ComponentType my_type;
  u32           my_priority;
  Entity       *my_entity;
  String        my_name;

public:
  Component(ComponentType type);
  virtual ~Component();

  void set_entity(Entity *entity);

  virtual void attach() = 0;
  virtual void detach() = 0;

  virtual uptr<Component> clone() const = 0;

  // volat len po welcome a pred goodbye
  Entity& entity() const;

  World& world() const;

  Core& core() const;

  WorldProcessorsRef processors() const;

  ComponentType type() const;

  const String& name() const;

  void set_name(const String &name);
};

template<typename T>
ComponentType component_type_of()
{
  return ComponentType::UNKNOWN;
}

#define MAP_COMPONENT_TYPE(type, mapped)  \
  template<>                    \
  inline ComponentType component_type_of<type>()   \
  { return ComponentType::mapped; }



class GenericSlot : NonCopyable {
  Component    *my_parent;
  ComponentType my_type;
  String        my_name;  ///< component name

public:
  explicit GenericSlot(Component *component, ComponentType type,
    const String &name = String())
    : my_parent(component)
    , my_type(type)
    , my_name(name)
  {
    assert(component != nullptr && "Slot can't have nullptr parent component");
  }
};








}
