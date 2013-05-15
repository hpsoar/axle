#ifndef AXLE_GEOMETRY_VECTOR3_H
#define AXLE_GEOMETRY_VECTOR3_H

#include <string.h>

#include "axle/core/settings.h"
#include "axle/core/math.h"

namespace ax {

class Normal;
class Point;
class Vector3 {
 public:
  Vector3() : x(0), y(0), z(0) { } 
  Vector3(float x, float y, float z) : x(x), y(y), z(z) { }
  explicit Vector3(float f) : x(f), y(f), z(f) { }
  explicit Vector3(const Normal& n);
  explicit Vector3(const Point& p);

  float Length() const { return sqrt(SquareLength()); }
  float SquareLength() const { return x*x + y*y + z*z; }							

  Vector3 operator+(const Vector3& v) const {
    return Vector3(x + v.x, y + v.y, z + v.z); 
  }
  Vector3 &operator+=(const Vector3& v) {
    x += v.x; y += v.y; z += v.z; return *this; 
  }

  Vector3 operator-(const Vector3& v) const { 
    return Vector3(x - v.x, y - v.y, z - v.z); 
  }
  Vector3 &operator-=(const Vector3& v) { 
    x -= v.x; y -= v.y; z -= v.z; return *this; 
  }
  
  Vector3 operator*(const Vector3& v) const { 
    return Vector3( x * v.x, y * v.y, z * v.z ); 
  }
  Vector3 &operator*=(const Vector3& v) { 
    x *= v.x; y *= v.y; z *= v.z; return *this; 
  }
  Vector3 operator*(float f) const { return Vector3(x*f, y*f, z*f); }
  Vector3 &operator*=(float f) { x *= f; y *= f; z *= f; return *this; }

  Vector3 operator/(float f) const { return *this * (1/f); }
  Vector3 &operator/=(float f) { return *this *= (1/f); }

  Vector3 operator-() const { return Vector3(-x, -y, -z); }

  bool operator==(const Vector3& v) const { 
    return x == v.x && y == v.y && z == v.z; 
  }
  bool operator!=(const Vector3& v) const { 
    return x != v.x || y != v.y || z != v.z; 
  }

  float *ptr() { return &x; }

  const float *ptr() const { return &x; }

  float operator[](int i) const { return ptr()[i]; }

  float &operator[](int i) { return ptr()[i]; }

  float x, y, z;

  static const Vector3 kZero;
  static const Vector3 kUnitX;
  static const Vector3 kUnitY;
  static const Vector3 kUnitZ;
};	

inline Vector3 operator*(float f, const Vector3& v) { return v * f; }

inline Vector3 Cross(const Vector3& v1, const Vector3& v2) {
  return Vector3(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, 
      v1.x*v2.y - v1.y*v2.x);
}

inline float Dot(const Vector3& v1, const Vector3& v2) { 
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 Normalize(const Vector3& v) { return v / v.Length(); }
} // ax

#endif // AXLE_GEOMETRY_VECTOR3_H
