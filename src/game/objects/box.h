#pragma once

#include <core/world/entity.h>
#include <core/video/mesh_tree_node.h>

namespace atom {
namespace game {

uptr<Entity> create_box(World &world, Core &core);

uptr<Entity> create_static_box(World &world, Core &core);

uptr<Entity> create_simple_static_platform(World &world, Core &core);

}
}
