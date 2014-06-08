#pragma once

#include "core/component/component.h"
#include "core/math/vec.h"
#include "core/math/mat.h"

namespace atom {

class PixmapComponent : public Component {
public:
  explicit PixmapComponent(Entity &entity);
  void on_attach() override;

  void on_detach() override;

  const String& material() const;

  void set_model(const String &model);

  void set_material(const String &material);

  Vec2f size() const;

  void set_size(const Vec2f size);

  void set_uv(const Vec2f &start, const Vec2f &end);

  bool is_auto_transform() const;

  void set_auto_transform(bool enable);

  const Mat4f& transform() const;

  void set_transform(const Vec2f &position);

  bool visible() const;

  void set_visible(bool visible);

  sptr<MeshTreeNode> node() const;

  void set_node(const sptr<MeshTreeNode> &node);

  void set_vs(VideoService *vs);

  bool is_inverted_horizontal() const;

  void set_invert_horizontal(bool invert);

private:
  VideoService *my_vs;
  bool   my_is_visible;
  bool   my_auto_transform;
  Mat4f  my_transform;
  String my_material;
  Vec2f  my_size;
  Vec2f  my_uv_start;
  Vec2f  my_uv_end;
  sptr<MeshTreeNode>  my_node;
  bool my_invert_horizontal;
};

}
