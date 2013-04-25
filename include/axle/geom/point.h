#ifndef AXLE_GEOMETRY_POINT_H
#define AXLE_GEOMETRY_POINT_H

#include "axle/core/settings.h"
#include "axle/core/math.h"
#include "axle/geom/vector3.h"

namespace ax {
class Point {
 public:
  Point() : x(0), y(0), z(0) { }
  Point(float x, float y, float z) : x(x), y(y), z(z) { }
  explicit Point(float f) : x(f), y(f), z(f) { }

  Point operator+(const Vector3 &v) const {       
      return Point(x + v.x, y + v.y, z + v.z);
  }
  Point &operator+=(const Vector3 &v) {
      x += v.x; y += v.y; z += v.z;
      return *this;
  }

  Vector3 operator-(const Point &p) const {        
      return Vector3(x - p.x, y - p.y, z - p.z);
  }
  Point operator-(const Vector3 &v) const {
      return Point(x - v.x, y - v.y, z - v.z);
  }
  Point &operator-=(const Vector3 &v) {
      x -= v.x; y -= v.y; z -= v.z;
      return *this;
  }

  Point &operator+=(const Point &p) {
      x += p.x; y += p.y; z += p.z;
      return *this;
  }
  Point operator+(const Point &p) const {
      return Point(x + p.x, y + p.y, z + p.z);
  }

  Point operator*(float f) const {
      return Point(f*x, f*y, f*z);
  }
  Point &operator*=(float f) {
      x *= f; y *= f; z *= f;
      return *this;
  }
  
  Point operator/(float f) const { return *this * (1/f); }
  Point &operator/=(float f) { return *this *= (1/f); }

  float operator[](int i) const { return (&x)[i]; }
  float &operator[](int i) { return (&x)[i]; }

  bool operator==(const Point &p) const {
      return x == p.x && y == p.y && z == p.z;
  }
  bool operator!=(const Point &p) const {
      return x != p.x || y != p.y || z != p.z;
  }

  const float *ptr() const { return &x; }

  // Point Public Data
  float x, y, z;
};

inline float Distance(const Point &p1, const Point &p2) {
    return (p1 - p2).Length();
}

inline float DistanceSquared(const Point &p1, const Point &p2) {
    return (p1 - p2).SquareLength();
}

inline Point operator*(float f, const Point &p) {
    return p*f;
}

} // ax

#endif//AXLE_GEOMETRY_POINT_H
