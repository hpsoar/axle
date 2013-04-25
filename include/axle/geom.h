#ifndef AXLE_GEOM_H
#define AXLE_GEOM_H
#include <glm/glm.hpp>

namespace ax {
typedef glm::vec3 Point;
typedef glm::vec3 Normal;
typedef glm::vec3 Vector3;
typedef glm::vec4 Vector4;
typedef glm::mat4 Matrix4x4;
typedef glm::mat3 Matrix3x3;

template<typename T>
inline T Invert(const T &m) {
  return glm::inverse(m);
}

template<typename T>
inline T Transpose(const T &m) {
  return glm::transpose(m);
}

template<typename T>
inline T Normalize(const T &v) {
  return glm::normalize(v);
}
template<typename T>
inline T Cross(const T &v1, const T &v2) {
  return glm::cross(v1, v2);
}

inline Matrix3x3 NormalMatrix(const Matrix4x4 &mat) {
  //return Invert(Transpose(Matrix3x3(mat)));
return Matrix3x3(Transpose(Invert(mat)));
}

void Print(const Point &p, const char *name = "");
void Print(const Matrix4x4 &m, const char *name = "");
void Print(const Matrix3x3 &m, const char *name = "");
} // ax

inline glm::vec3 operator*(const ax::Matrix4x4 &m, const glm::vec3 &p) {
  return glm::vec3(m * glm::vec4(p, 1.0));
}

#include "geom/aabb.h"

#endif // AXLE_GEOM_H
