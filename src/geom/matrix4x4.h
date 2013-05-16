#ifndef AXLE_GEOMETRY_MATRIX4X4_H
#define AXLE_GEOMETRY_MATRIX4X4_H

#include "../core/settings.h"
#include "../geom/vector3.h"

namespace ax {
class Matrix4x4 {
public:
  Matrix4x4() { 
    m_[1] = m_[2] = m_[3] = m_[4] = m_[6] = m_[7] =
    m_[8] = m_[9] = m_[11] = m_[12] = m_[13] = m_[14] = 0;
    m_[0] = m_[5] = m_[10] = m_[15] = 1;
  }
  Matrix4x4(const float data[16]);
  Matrix4x4(const float data[4][4]);
  Matrix4x4(float t00, float t01, float t02, float t03,
         float t10, float t11, float t12, float t13,
         float t20, float t21, float t22, float t23,
         float t30, float t31, float t32, float t33);
  float *operator[](int row) { return &m_[4*row]; }
  const float *operator[](int row) const { return &m_[4*row]; }
  float *ptr() { return m_; }
  const float *ptr() const { return m_; }
private:
  float m_[16];
  static const int kDataSize = 16 * sizeof(float);
  static const float kZeroArray[16];
  static const float kIdentityArray[16];
  friend Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2);
  friend Matrix4x4 Transpose(const Matrix4x4 &m);
  friend Matrix4x4 Invert(const Matrix4x4 &m);
};

Matrix4x4 Mul(const Matrix4x4 &m1, const Matrix4x4 &m2);
Matrix4x4 Transpose(const Matrix4x4 &m);
Matrix4x4 Invert(const Matrix4x4 &m);

Matrix4x4 Translate(const Vector3 &delta);
Matrix4x4 Scale(float x, float y, float z);
Matrix4x4 RotateX(float angle);
Matrix4x4 RotateY(float angle);
Matrix4x4 RotateZ(float angle);
Matrix4x4 Rotate(float angle, const Vector3 &axis);

Vector3 Transform(const Vector3 &v, const Matrix4x4 &m);
Point Transform(const Point &p, const Matrix4x4 &m);
} // ax

#endif // AXLE_GEOMETRY_MATRIX4X4_H
