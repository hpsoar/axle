#include "triangle_mesh.h"
#include <map>

namespace ax {
//void TriangleMesh::GenAdjacencyIndices() {
  /*if (this->adj_indices_.size() > 0) {
    ax::Logger::Log("TriangleMesh::GenAdjacencyIndices:adjacency indices already generated");
    return;
  }

  typedef std::vector<uint32> TriangleList;
  typedef std::map<Edge, TriangleList> EdgeTriangles;
  EdgeTriangles edge_triangles;
  for (uint32 ti = 0; ti < this->n_triangles(); ++ti) {
    ax::ITriangle tri = this->GetITraingle(ti);    
    for (uint32 i = 0; i < 3; ++i) {      
      Edge e(tri.vidx(i), tri.vidx(i + 1));
      edge_triangles[e].push_back(ti);   
    }
  }
  
  this->adj_indices_.reserve(this->n_triangles() * 6);

  for (uint32 ti = 0; ti < this->n_triangles(); ++ti) {
    ax::ITriangle tri = this->GetITraingle(ti);
    for (int i = 0; i < 3; ++i) {
      int adj_vert = 0;
      Edge e(tri.vidx(i), tri.vidx(i + 1));

      auto triangles = edge_triangles[e];
      auto it = triangles.begin();
      while (it != triangles.end() && *it == ti) ++it;
      if (it == triangles.end()) {
        adj_vert = tri.vidx(i);
      }
      else {
        ax::ITriangle tri2 = this->GetITraingle(*it);
        adj_vert = tri2.vidx(0);
        for (int j = 0; j < 3; ++j) {
          if (!e.Contains(tri2.vidx(j))) {
            adj_vert = tri2.vidx(j);
            break;
          }
        }
      }
      this->adj_indices_.push_back(tri.vidx(i));
      this->adj_indices_.push_back(adj_vert);
    }
  }*/
//}

TriangleMesh::~TriangleMesh() {  
}

void TriangleMesh::ScaleUV(float uscale, float vscale) {
  RET(this->has_tcoord());
  for (size_t i = 0; i < this->n_vertices(); ++i) {
    tcoords_[i*2] *= uscale;
    tcoords_[i*2 + 1] *= vscale;
  }
}

void TriangleMesh::ApplyTransform(const ax::Matrix4x4 &m) {
  Matrix3x3 normal_mat = NormalMatrix(m);
  for (size_t i = 0; i < this->n_vertices(); ++i) {    
    float *vptr = this->vertex(i);
    ax::Vector3 v(vptr[0], vptr[1], vptr[2]);
    v = m * v;
    vptr[0] = v.x; vptr[1] = v.y; vptr[2] = v.z;

    float *nptr = this->normal(i);
    ax::Normal n(nptr[0], nptr[1], nptr[2]);
    n = ax::Normalize(normal_mat * n);
    nptr[0] = n.x; nptr[1] = n.y; nptr[2] = n.z;   
  }
  bound_need_update_ = true;
}

void TriangleMesh::ComputeBound() const {
  bound_ = AABB();
  for (size_t i = 0; i < this->n_vertices(); ++i) {
    const float *vptr = this->vertex(i);
    bound_.Union(ax::Vector3(vptr[0], vptr[1], vptr[2]));
  }
  bound_need_update_ = false;
}

AABB TriangleMesh::Bound() const {
  if (bound_need_update_) this->ComputeBound();
  return bound_;
}
} // ax
