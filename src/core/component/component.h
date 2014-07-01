#pragma once

#include <vector>
#include "core/corefwd.h"
#include "core/noncopyable.h"
#include "core/meta.h"
#include "core/string.h"

namespace atom {

class Component : NonCopyable {
public:
  explicit Component(Entity &entity);
  virtual ~Component();

  virtual void attach() = 0;
  virtual void detach() = 0;

  // volat len po welcome a pred goodbye
  Entity& entity() const;

  World& world() const;

  Core& core() const;

  WorldProcessorsRef processors() const;

public:
  u32        my_priority;
  Entity    &my_entity;
};

}
