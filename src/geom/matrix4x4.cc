#include "axle/geom/vector3.h"
#include "axle/geom/point.h"
#include "axle/geom/normal.h"
#include "axle/geom/matrix4x4.h"

namespace ax {
const float Matrix4x4::kZeroArray[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
const float Matrix4x4::kIdentityArray[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
Matrix4x4::Matrix4x4(const float data[16]) { memcpy(this->m_, data, kDataSize); }
Matrix4x4::Matrix4x4(const float data[4][4]) { memcpy(this->m_, data, kDataSize); }
Matrix4x4::Matrix4x4(float t00, float t01, float t02, float t03,
               float t10, float t11, float t12, float t13, 
               float t20, float t21, float t22, float t23, 
               float t30, float t31, float t32, float t33) {
  m_[0] = t00; m_[1] = t01; m_[2] = t02; m_[3] = t03;
  m_[4] = t10; m_[5] = t11; m_[6] = t12; m_[7] = t13;
  m_[8] = t20; m_[9] = t21; m_[10] = t22; m_[11] = t23;
  m_[12] = t30; m_[13] = t31; m_[14] = t32; m_[15] = t33;
}

Matrix4x4 Invert(const Matrix4x4 &m) {
  float inverse[16];
  memcpy(inverse, Matrix4x4::kIdentityArray, Matrix4x4::kDataSize);
  float t;
  int i, j, k, swap;
  float tmp[4][4];
  // This can probably be done with 'memcpy( tmp, mat, 16*sizeof(float) );'
  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      tmp[i][j] = m[i][j];
  for (i = 0; i < 4; i++) {
    /* look for largest element in column. */
    swap = i;
    for (j = i + 1; j < 4; j++)
      if (fabs(tmp[j][i]) > fabs(tmp[i][i]))
        swap = j;
    if (swap != i) {
      /* swap rows. */
      for (k = 0; k < 4; k++) {
        t = tmp[i][k];
        tmp[i][k] = tmp[swap][k];
        tmp[swap][k] = t;

        t = inverse[i*4+k];
        inverse[i*4+k] = inverse[swap*4+k];
        inverse[swap*4+k] = t;
      }
    }
    if (tmp[i][i] == 0) 
      return Matrix4x4(Matrix4x4::kZeroArray);
    t = tmp[i][i];
    for (k = 0; k < 4; k++) {
      tmp[i][k] /= t;
      inverse[i*4+k] /= t;
    }
    for (j = 0; j < 4; j++)
      if (j != i) {
        t = tmp[j][i];
        for (k = 0; k < 4; k++) {
          tmp[j][k] -= tmp[i][k]*t;
          inverse[j*4+k] -= inverse[i*4+k]*t;
        }
      }
  }
  return Matrix4x4(inverse);
}

Matrix4x4 Transpose(const Matrix4x4 &m) {
  return Matrix4x4(m.m_[0], m.m_[4], m.m_[8], m.m_[12],
                m.m_[1], m.m_[5], m.m_[9], m.m_[13], 
                m.m_[2], m.m_[6], m.m_[10], m.m_[14], 
                m.m_[3], m.m_[7], m.m_[11], m.m_[15]);
}

Matrix4x4 Mul( const Matrix4x4 &m1, const Matrix4x4 &m2 ) {
  Matrix4x4 res;
  for (int c = 0; c < 4; c++) {
    for (int r = 0; r < 4; r++) {
      res[r][c] = m1[r][0] * m2[0][c] + m1[r][1] * m2[1][c] +
                  m1[r][2] * m2[2][c] + m1[r][3] * m2[3][c];
    }
  }
  return res;
}

Matrix4x4 Translate(const Vector3 &delta) {
  return Matrix4x4(1, 0, 0, delta.x, 
                0, 1, 0, delta.y, 
                0, 0, 1, delta.z, 
                0, 0, 0,       1);
}
Matrix4x4 Scale(float x, float y, float z) {
  return Matrix4x4(x, 0, 0, 0, 
                0, y, 0, 0, 
                0, 0, z, 0, 
                0, 0, 0, 1);
}
Matrix4x4 RotateX(float angle) {
  float rad = ax::deg2rad(angle);
  float sin_t = sinf(rad), cos_t = cosf(rad);
  return Matrix4x4(1,     0,      0, 0, 
                0, cos_t, -sin_t, 0, 
                0, sin_t,  cos_t, 0, 
                0,     0,      0, 1);
}
Matrix4x4 RotateY(float angle) {
  float rad = ax::deg2rad(angle);
  float sin_t = sinf(rad), cos_t = cosf(rad);
  return Matrix4x4(cos_t,   0, sin_t, 0, 
                    0,   1,     0, 0, 
               -sin_t,   0, cos_t, 0, 
                    0,   0,     0, 1);
}
Matrix4x4 RotateZ(float angle) {
  float rad = ax::deg2rad(angle);
  float sin_t = sinf(rad), cos_t = cosf(rad);
  return Matrix4x4(cos_t, -sin_t, 0, 0,
                sin_t,  cos_t, 0, 0, 
                    0,      0, 1, 0, 
                    0,      0, 0, 1);
}
Matrix4x4 Rotate(float angle, const Vector3 &axis) {
  Vector3 a = Normalize(axis);
  float rad = ax::deg2rad(angle);
  float s = sinf(rad), c = cosf(rad);
  float m[4][4];
  m[0][0] = a.x * a.x + (1.f - a.x * a.x) * c;
  m[0][1] = a.x * a.y * (1.f - c) - a.z * s;
  m[0][2] = a.x * a.z * (1.f - c) + a.y * s;
  m[0][3] = 0;
  m[1][0] = a.x * a.y * (1.f - c) + a.z * s;
  m[1][1] = a.y * a.y + (1.f - a.y * a.y) * c;
  m[1][2] = a.y * a.z * (1.f - c) - a.x * s;
  m[1][3] = 0;
  m[2][0] = a.x * a.z * (1.f - c) - a.y * s;
  m[2][1] = a.y * a.z * (1.f - c) + a.x * s;
  m[2][2] = a.z * a.z + (1.f - a.z * a.z) * c;
  m[2][3] = 0;
  m[3][0] = 0;
  m[3][1] = 0;
  m[3][2] = 0;
  m[3][3] = 1;
  return Matrix4x4(m);
}

Vector3 Transform(const Vector3 &v, const Matrix4x4 &m) {
  float x = v.x, y = v.y, z = v.z;
  return Vector3(m[0][0]*x + m[0][1]*y + m[0][2]*z, 
                 m[1][0]*x + m[1][1]*y + m[1][2]*z, 
                 m[2][0]*x + m[2][1]*y + m[2][2]*z);
}
Point Transform(const Point &p, const Matrix4x4 &m) {
  float x = p.x, y = p.y, z = p.z;
  float xp = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
  float yp = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
  float zp = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
  float wp = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3];
  assert(wp != 0);
  // convert the point back to inhomogeneous coordinates
  if (wp == 1.) return Point(xp, yp, zp);
  else          return Point(xp, yp, zp)/wp;
}

} // ax

