#ifndef AXLE_GEOMETRY_NORMAL_H
#define AXLE_GEOMETRY_NORMAL_H

#include "../core/settings.h"
#include "../geom/vector3.h"

namespace ax {
class Vector3;
class Normal {
 public:
   // Normal Public Methods
  Normal() : x(0), y(0), z(0) { }
  Normal(float x, float y, float z) : x(x), y(y), z(z) { }
  explicit Normal(const Vector3 &v);

  Normal operator-() const { return Normal(-x, -y, -z); }

  Normal operator+ (const Normal &n) const {
      return Normal(x + n.x, y + n.y, z + n.z);
  }
  Normal& operator+=(const Normal &n) {
      x += n.x; y += n.y; z += n.z;
      return *this;
  }

  Normal operator- (const Normal &n) const {
      return Normal(x - n.x, y - n.y, z - n.z);
  }
  Normal& operator-=(const Normal &n) {
      x -= n.x; y -= n.y; z -= n.z;
      return *this;
  }

  Normal operator*(float f) const { return Normal(f*x, f*y, f*z); }
  Normal &operator*=(float f) { x *= f; y *= f; z *= f; return *this; }

  Normal operator/(float f) const { return *this * (1/f); }
  Normal &operator/=(float f) { return *this *= (1/f); }

  float LengthSquared() const { return x*x + y*y + z*z; }
  float Length() const { return sqrt(LengthSquared()); }
  
  float operator[](int i) const { return (&x)[i]; }
  float &operator[](int i) { return (&x)[i]; }

  bool operator==(const Normal &n) const {
      return x == n.x && y == n.y && z == n.z;
  }
  bool operator!=(const Normal &n) const {
      return x != n.x || y != n.y || z != n.z;
  }

  // Normal Public Data
  float x, y, z;
};

inline Normal operator*(float f, const Normal &n) {
    return Normal(f*n.x, f*n.y, f*n.z);
}

inline Normal Normalize(const Normal &n) {
    return n / n.Length();
}

inline float Dot(const Normal &n1, const Vector3 &v2) {
    return n1.x * v2.x + n1.y * v2.y + n1.z * v2.z;
}

inline float Dot(const Vector3 &v1, const Normal &n2) {
    return v1.x * n2.x + v1.y * n2.y + v1.z * n2.z;
}

inline float Dot(const Normal &n1, const Normal &n2) {
    return n1.x * n2.x + n1.y * n2.y + n1.z * n2.z;
}

inline float AbsDot(const Normal &n1, const Vector3 &v2) {
    return fabs(n1.x * v2.x + n1.y * v2.y + n1.z * v2.z);
}


inline float AbsDot(const Vector3 &v1, const Normal &n2) {
    return fabs(v1.x * n2.x + v1.y * n2.y + v1.z * n2.z);
}


inline float AbsDot(const Normal &n1, const Normal &n2) {
    return fabs(n1.x * n2.x + n1.y * n2.y + n1.z * n2.z);
}


inline Normal Faceforward(const Normal &n, const Vector3 &v) {
    return (Dot(n, v) < 0.f) ? -n : n;
}

inline Normal Faceforward(const Normal &n, const Normal &n2) {
    return (Dot(n, n2) < 0.f) ? -n : n;
}

inline Vector3 Faceforward(const Vector3 &v, const Vector3 &v2) {
    return (Dot(v, v2) < 0.f) ? -v : v;
}

inline Vector3 Faceforward(const Vector3 &v, const Normal &n2) {
    return (Dot(v, n2) < 0.f) ? -v : v;
}
} // ax

#endif//AXLE_GEOMETRY_NORMAL_H
