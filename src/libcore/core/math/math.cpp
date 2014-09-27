#include "math.h"

namespace atom {

Vec4f plane_from_vertices(
  const Vec3f &v0,
  const Vec3f &v1,
  const Vec3f &v2)
{
  Vec3f n = cross_product(v1 - v0, v2 - v0);
  float    d = - (n[0] * v0[0] + n[1] * v0[1] * n[2] * v0[2]);
  return Vec4f(n[0], n[1], n[2], d);
}

/**
 * Hladanie priesecnika pomalou metodou, cize vypocet s kazdym objektom a vyber
 * najblizsieho. Sluzi hlavne pre testovacie ucely. Pretoze v kode sa pouziva
 * ukazovatel na tuto metodu, je implementovana ako staticka metoda, a nie ako
 * clenska metoda.
 *
 * @param scene ukazovatel na scenu
 * @param snapshot index casoveho snimku
 * @param aRay luc
 * @param aRayHit info o dopade luca
 * @param tmax udava maximalnu vzdialenost objektu
 */
//MShape * MScene::slowFindNearest(MScene *scene, unsigned snapshot, const MRay &aRay, MRayHit &aRayHit, MFloat tmax)
//{
//    MShape *nearest = NULL;
//    MFloat        ndist = MFloatMax;
//    MShapeList  &shapes = scene->_shapes;
//    unsigned      count = shapes.size();
//    MFloat            d = MFloatMax;
//    MRayHit      rayHit;
//    bool res;
//    MRay ray;

//    for (unsigned i = 0; i < count; i++) {
//        unsigned time = min(shapes[i]->capacity - 1, snapshot);
//        const MMatrix4D &t = shapes[i]->itransforms[time]; //MInverse(_primitives[i]->_transform) * icamera;
//        ray = MRay(MTransformPoint(t, aRay.o), MTransformVector(t, aRay.d));
//        res = shapes[i]->intersect(rayHit, ray, d);


//        scene->_raysCount++;

//        if (res && rayHit.distance < ndist) {
//            if (nearest != NULL)
//                scene->_raysMissedCount++;

//            nearest = shapes[i];
//            aRayHit = rayHit;
//            ndist   = rayHit.distance;
//        }
//        else
//            scene->_raysMissedCount++;
//    }

//    // transform coordinates
//    if (nearest != NULL) {
//        unsigned time = min(nearest->capacity - 1, snapshot);
//        const MMatrix4D &t  = nearest->transforms[time];
//        aRayHit.point  = MTransformPoint(t, aRayHit.point);
//        aRayHit.normal = MTransformVector(t, aRayHit.normal);
//    }

//    return nearest;
//}

u32 dominant_axis(const Vec3f &n)
{
  if (abs(n.x) > abs(n.y)) {
    return abs(n.x) > abs(n.z) ? 0 : 2;
  } else {
    return abs(n.y > abs(n.z)) ? 1 : 2;
  }
}

f32 intersect_triangle(const Ray &ray, const Vec3f &v0, const Vec3f &v1, const Vec3f &v2)
{
  const f32 tmin = 0.001f;
  const f32 tmax = F32_MAX;
  Vec3f b = v2 - v0;
  Vec3f c = v1 - v0;
  Vec3f n = cross_product(c, b);
  // calculate ray plane intersection
  f32 t = -dot_product(ray.origin - v0, n) / dot_product(ray.dir, n);
  // distance test
  if (t <= tmin || t >= tmax) {
    return -1;
  }
  // determine triangle largest 2d projection plane
  u32 k = dominant_axis(n);
  u32 u = (k + 1) % 3;
  u32 v = (k + 2) % 3;
  // calculate hitpoint
  f32 hu = ray.origin[u] + t * ray.dir[u];
  f32 hv = ray.origin[v] + t * ray.dir[v];
  f32 bu = b[u];
  f32 bv = b[v];
  f32 cu = c[u];
  f32 cv = c[v];
  f32 div = bu * cv - bv * cu;
  // calculate & check 2nd barycentric coeficient (1st is 1)
  f32 beta = (bu * hv - bv * hu) / div;
  if (beta < 0) {
    return -1;
  }
  // calculate & check 3rd barycentric coeficient
  f32 gamma = (cv * hu - cu * hv) / div;
  if (gamma < 0 || beta + gamma > 1) {
    return -1;
  }
  // hit
  return t;
}

f32 intersect_mesh(const Ray &ray, const Slice<Vec3f> &vertices, const Slice<u32> &indices, u32 &index)
{
  return -1;
}

bool intersect_bounding_box(const Ray &ray, const BoundingBox &box, f32 &tnear, f32 &tfar)
{
  f32 tmin, tmax, tymin, tymax, tzmin, tzmax;
  // division here is used to handle -0, -inf correctly
  f32 divx = 1 / ray.dir.x;
  f32 divy = 1 / ray.dir.y;

  if (divx >= 0) {
    tmin = (box.xmin - ray.origin.x) * divx;
    tmax = (box.xmax - ray.origin.x) * divx;
  } else {
    tmin = (box.xmax - ray.origin.x) * divx;
    tmax = (box.xmin - ray.origin.x) * divx;
  }
  // y
  if (divy >= 0) {
    tymin = (box.ymin - ray.origin.y) * divy;
    tymax = (box.ymax - ray.origin.y) * divy;
  } else {
    tymin = (box.ymax - ray.origin.y) * divy;
    tymax = (box.ymin - ray.origin.y) * divy;
  }

  if ((tmin > tymax) || (tmax < tymin)) {
    return false;
  }
  // z
  f32 divz = 1 / ray.dir.z;

  if (divz >= 0) {
    tzmin = (box.zmin - ray.origin.z) * divz;
    tzmax = (box.zmax - ray.origin.z) * divz;
  } else {
    tzmin = (box.zmax - ray.origin.z) * divz;
    tzmax = (box.zmin - ray.origin.z) * divz;
  }

  tmin = max(tmin, tymin);
  tmax = min(tmax, tymax);

  if ((tmin > tzmax) || (tmax < tzmin)) {
    return false;
  }

  tmin = max(tmin, tzmin);
  tmax = min(tmax, tzmax);

  tnear = tmin;
  tfar = tmax;
  return true;
}

f32 intersect_bounding_box(const Ray &ray, const BoundingBox &box)
{
  f32 tmin, tmax;
  if (!intersect_bounding_box(ray, box, tmin, tmax)) {
    return -1;
  }

  return tmin >= 0 ? tmin : -1;
}

void BoundingBox::extend(const Vec3f &v)
{
  xmin = min(v.x, xmin);
  xmax = max(v.x, xmax);
  ymin = min(v.y, ymin);
  ymax = max(v.y, ymax);
  zmin = min(v.z, zmin);
  zmax = max(v.z, zmax);
}

}
