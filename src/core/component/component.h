#pragma once

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
public:
  Component(ComponentType type, Entity &entity);
  virtual ~Component();

  virtual void attach() = 0;
  virtual void detach() = 0;

  // volat len po welcome a pred goodbye
  Entity& entity() const;

  World& world() const;

  Core& core() const;

  WorldProcessorsRef processors() const;
  
  ComponentType type() const;
  
  const String& name() const;
  
  void set_name(const String &name);

public:
  ComponentType my_type;
  u32           my_priority;
  Entity       &my_entity;
  String        my_name;
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

}
