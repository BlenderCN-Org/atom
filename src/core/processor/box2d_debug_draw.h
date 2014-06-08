#pragma once

#include <Box2D/Box2D.h>
#include <vector>
#include "core/corefwd.h"
#include "core/math/vec.h"

namespace atom {

class Box2DDebugDraw : public b2Draw {
public:
  Box2DDebugDraw(VideoService &vs, ResourceService &rs);
  ~Box2DDebugDraw();

  void reset();
  void draw(const Camera &camera);

  /// Draw a closed polygon provided in CCW order.
  void DrawPolygon(const b2Vec2* vertices, int32 count, const b2Color& color) override;

  /// Draw a solid closed polygon provided in CCW order.
  void DrawSolidPolygon(const b2Vec2* vertices, int32 count, const b2Color& color) override;

  /// Draw a circle.
  void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;

  /// Draw a solid circle.
  void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis,
    const b2Color& color) override;

  /// Draw a line segment.
  void DrawSegment(const b2Vec2& a, const b2Vec2& b, const b2Color& color) override;

  /// Draw a transform. Choose your own length scale.
  /// @param xf a transform.
  void DrawTransform(const b2Transform& xf) override;

  static const f32 DEFAULT_Z;

private:
  VideoService       &my_vs;
  TechniqueResourcePtr   my_shader;
  std::vector<Vec3f>  my_vertices;
};


}
