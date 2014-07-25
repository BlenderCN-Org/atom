#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

namespace atom {

// vector specialization for most useful types
typedef Vec2<uint32_t> Vec2u;
typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

static_assert(sizeof(Vec2u) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec2f) == 8, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec3f) == 12, "Velkost musi byt 8bytov");
static_assert(sizeof(Vec4f) == 16, "Velkost musi byt 8bytov");

MAP_TYPE(Vec2f, VEC2F)
MAP_TYPE(Vec3f, VEC3F)
MAP_TYPE(Vec4f, VEC4F)

}
