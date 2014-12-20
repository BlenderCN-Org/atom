#include "mesh_tree.h"
#include <algorithm>

namespace atom {

void MeshTree::add_node(const sptr<MeshTreeNode> &node)
{
  my_nodes.push_back(node);
}

void MeshTree::remove_node(const sptr<MeshTreeNode> &node)
{
  my_nodes.erase(std::remove(my_nodes.begin(), my_nodes.end(), node), my_nodes.end());
}

const std::vector<sptr<MeshTreeNode> > MeshTree::all_nodes() const
{
  return my_nodes;
}

}
