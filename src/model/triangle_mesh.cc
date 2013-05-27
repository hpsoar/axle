#include "../model/triangle_mesh.h"
#include <map>

namespace ax {
struct Edge {
  Edge(uint32 v0, uint32 v1) {
    this->v0 = std::min(v0, v1);
    this->v1 = std::max(v0, v1);
  }
  bool operator<(const Edge &rhs) const {
    return v0 == rhs.v0 ? v1 < rhs.v1 : v0 < rhs.v0;
  }
  bool Contains(uint32 v) const {
    return this->v0 == v || this->v1 == v;
  }
  union {
    uint32 idx[2];
    struct {
      uint32 v0, v1;
    };
  };
private:
  Edge();
};

void TriangleMesh::GenAdjacencyIndices() {
  if (this->adj_indices_.size() > 0) {
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
  }
}

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
