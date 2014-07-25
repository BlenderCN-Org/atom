#pragma once

#include "mesh_tree_node.h"

namespace atom {

class MeshTree {
public:
  void add_node(const sptr<MeshTreeNode> &node);

  void remove_node(const sptr<MeshTreeNode> &node);

  const std::vector<sptr<MeshTreeNode>> all_nodes() const;

private:
  std::vector<sptr<MeshTreeNode>> my_nodes;
};

class SceneNode : private NonCopyable {
public:
  virtual void draw() = 0;

  int layer() const;

private:
  int my_layer;
};

inline bool operator<(const SceneNode &a, const SceneNode &b)
{
  return a.layer() < b.layer();
}

inline bool operator<(const sptr<SceneNode> &a, const sptr<SceneNode> &b)
{
  return operator<(*a, *b);
}

class SceneTree {
public:
  void add_node(const sptr<SceneNode> &node);

private:
  std::vector<sptr<SceneNode>> my_nodes;

};

}
