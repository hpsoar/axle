#include "../model/triangle_mesh.h"

namespace ax {
TriangleMesh::~TriangleMesh() {
  delete[] vertices_;
  delete[] normals_;
  delete[] tcoords_;
  delete[] indices_;
  n_triangles_ = 0; 
  n_vertices_ = 0;
}

void TriangleMesh::ScaleUV(float uscale, float vscale) {
  RET(this->has_tcoord());
  for (size_t i = 0; i < n_vertices_; ++i) {
    tcoords_[i*2] *= uscale;
    tcoords_[i*2 + 1] *= vscale;
  }
}

void TriangleMesh::ApplyTransform(const ax::Matrix4x4 &m) {
  Matrix3x3 normal_mat = NormalMatrix(m);
  for (size_t i = 0; i < n_vertices_; ++i) {
    this->set_vertex(i, m * this->vertex(i));
    this->set_normal(i, Normalize(normal_mat * this->normal(i)));
  }
  bound_need_update_ = true;
}

void TriangleMesh::ComputeBound() const {
  bound_ = AABB();
  for (size_t i = 0; i < n_vertices_; ++i) {
    bound_.Union(vertices_[i]);
  }
  bound_need_update_ = false;
}

AABB TriangleMesh::Bound() const {
  if (bound_need_update_) this->ComputeBound();
  return bound_;
}
} // ax
