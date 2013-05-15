#ifndef AXLE_GEOMETRY_AABB_H
#define AXLE_GEOMETRY_AABB_H

#include "axle/core/settings.h"

#include <string.h> //for memcpy

#include "axle/core/math.h"
#include "axle/geom.h"


namespace ax {
struct AABB {
public:  
  AABB() : minP(INFINITY, INFINITY, INFINITY), 
  maxP( -INFINITY, -INFINITY, -INFINITY ) { }

  AABB(const Point& minP, const Point& maxP) : minP( minP ), 
  maxP( maxP ) { }

  AABB(const AABB& box) : minP(box.minP), maxP(box.maxP) { }

  AABB& operator=(const AABB& box) {
    memcpy(this, &box, sizeof(AABB)); return *this;
  }

  Vector3 size() const { return maxP - minP; }

  void Expand(float delta) {
    Vector3 size(delta, delta, delta);
    minP -= size; maxP += size;
  }

  void Union(const Point& point) {
    minP.x = min(minP.x, point.x);
    minP.y = min(minP.y, point.y);
    minP.z = min(minP.z, point.z);

    maxP.x = max(maxP.x, point.x);
    maxP.y = max(maxP.y, point.y);
    maxP.z = max(maxP.z, point.z);
  }

  void Union(const AABB& box) {
    this->Union(box.minP); 
    this->Union(box.maxP);
  }		

  void Clamp(const AABB& box) {
    minP.x = max(minP.x, box.minP.x);
    minP.y = max(minP.y, box.minP.y);
    minP.z = max(minP.z, box.minP.z);
    maxP.x = min(maxP.x, box.maxP.x);
    maxP.y = min(maxP.y, box.maxP.y);
    maxP.z = min(maxP.z, box.maxP.z);
  }  

  bool Contains(const Point& point) const {
    return ((point.x >= minP.x) && (point.x <= maxP.x) &&
      (point.y >= minP.y) && (point.y <= maxP.y) &&
      (point.z >= minP.z) && (point.z <= maxP.z));
  }

  Point &operator[](int i) { return (&minP)[i]; }
  Point operator[](int i) const { return (&minP)[i]; }

  Point minP, maxP;
};
} // ax

#endif // AXLE_GEOMETRY_AABB_H
