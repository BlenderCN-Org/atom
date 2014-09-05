#include "mesh_tree_node.h"

namespace atom {

MeshTreeNode::MeshTreeNode()
  : transformations(Mat4f())
  , visible(true)
{
}

MeshTreeNode::~MeshTreeNode()
{
}

}
