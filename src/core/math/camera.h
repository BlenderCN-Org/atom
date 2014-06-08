#pragma once

#include "core/math/mat.h"
#include "core/math/quaternion.h"
#include "core/log.h"

namespace atom {

class Camera {
public:
  enum class Type {
    ORTHOGRAPHIC,
    PERSPECTIVE
  };

//  static Camera orhtographic(float width, float height, float near, float far);

  static Camera perspective(float fov, float aspect, float near, float far);

  /**
   * Vytvor perspektivne projekcnu kameru.
   */
  Camera();

  void set_orthographic(
    float xmin,
    float xmax,
    float ymin,
    float ymax,
    float near,
    float far);

  /**
   *
   * @param fov sirka rozhladu v radianoch (1.57 je 90stupnov)
   */
  void set_perspective(float fov, float aspect, float near, float far);

  /**
   * Nastav poziciu kamery.
   *
   * @param x pozicia x
   * @param y pozicia y
   * @param z pozicia z
   */
  void set_position(float x, float y, float z)
  {
    my_position = Vec3f(x, y, z);
  }

  /**
   * Zmena pohladoveho ulhu. K aktualne nastavenemu uhlu pripocitaj hodnoty.
   *
   * @param rx rotacia kamery podla x-ovej osi
   * @param ry rotacia kamery podla y-ovej osi
   * @param rz rotacia kamery podla z-ovej osi
   */
  void rotate_camera(float yaw, float pitch, float roll);

  /**
   * Presun kameru (v globalnych suradniciach).
   *
   * @param tx posnun po x-ovej osi
   * @param ty posnun po y-ovej osi
   * @param tz posnun po z-ovej osi
   */
  void translate(float tx, float ty, float tz)
  {
    my_position += Vec3f(tx, ty, tz);
  }

  /**
   * Vypocitaj maticu kamery (vhodnu pre pouzite do TransformationStack).
   * @note nezabudnut invertovat hodnotu tejto pohladovej matice
   *
   * @return transformacna matica kamery
   */
  Mat4f view_matrix() const;

  Mat4f projection_matrix() const;

  Mat4f infinite_perspective_projection_matrix() const;

  Mat4f rotation_matrix() const;

  Vec3f up() const;
  Vec3f forward() const;
  Vec3f right() const;

  Vec3f position() const
  {
    return my_position;
  }

  float fov() const
  {
    return my_fov;
  }

private:
  Quatf calculate_rotation() const;

public:
  Type     my_type;
  float    my_near;
  float    my_far;
  float    my_fov;         ///< field of view used for perspective camera
  float    my_aspect;      ///< aspect ration used for perspective camera
  float    my_xmin;
  float    my_xmax;
  float    my_ymin;
  float    my_ymax;
  float    my_yaw;
  float    my_pitch;
  float    my_roll;
  Vec3f my_position;
  Mat4f my_projection;  ///< predpocitana projekcna matica
};

typedef sptr<Camera> CameraPtr;


}
