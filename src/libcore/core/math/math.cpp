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


f32 intersect(const Ray &ray, const Slice<Vec3f> &vertices, const Slice<u32> &indices, u32 &index)
{
  return -1;
}

bool intersect(const Ray &ray, const BoundingBox &box, f32 &tnear, f32 &tfar)
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

f32 intersect(const Ray &ray, const BoundingBox &box)
{
  f32 tmin, tmax;
  if (!intersect(ray, box, tmin, tmax)) {
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
