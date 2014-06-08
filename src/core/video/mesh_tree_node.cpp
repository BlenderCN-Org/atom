#include "mesh_tree_node.h"

namespace atom {

MeshTreeNode::MeshTreeNode()
  : transformations(Mat4f::identity())
  , visible(true)
{
}

MeshTreeNode::~MeshTreeNode()
{
}

}
