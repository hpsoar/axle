#include "../geom.h"

#include <stdio.h>

namespace ax {
void Print(const Point &p, const char *name) {
  printf("%s, (%f %f %f)\n", name, p.x, p.y, p.z);
}

void Print(const Matrix4x4 &m, const char *name){
  for (int i = 0; i < 4; ++i) {
    printf("%f %f %f %f\n", m[i][0], m[i][1], m[i][2], m[i][3]);
  }
}

void Print(const Matrix3x3 &m, const char *name) {
  for (int i = 0; i < 3; ++i) {
    printf("%f %f %f\n", m[i][0], m[i][1], m[i][2]);
  }
}
} // ax
