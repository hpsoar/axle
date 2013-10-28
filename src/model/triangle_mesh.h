#ifndef AXLE_MODEL_TRIANGLEMESH_H
#define AXLE_MODEL_TRIANGLEMESH_H

#include "../core.h"
#include "../geom/aabb.h"
#include <vector>

namespace ax {
class TriangleMesh;
typedef std::tr1::shared_ptr<TriangleMesh> TriMeshPtr;

struct ITriangle {
  ITriangle(const uint32 *vidxs) {
    memcpy(this->vidxs, vidxs, 3 * sizeof(uint32));
  }
  uint32 vidx(uint32 i) const { 
    return this->vidxs[i % 3];
  }
  union {
    uint32 vidxs[3];
    struct {
      uint32 x, y, z;
    };
  };
};

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

struct VertIdx {
  static const int kNIdxs = 3;

  VertIdx(uint32 pidx, uint32 nidx, uint32 tidx) : pidx(pidx), nidx(nidx), tidx(tidx) { }
  VertIdx() : pidx(0), nidx(0), tidx(0) { }

  bool operator<(const VertIdx &rhs) const {
    for (int i = 0; i < kNIdxs; ++i) {
      if (idxs[i] < rhs.idxs[i]) return true;
      else if (idxs[i] > rhs.idxs[i]) return false;
    }
    return false;
  }
  
  union {
    struct {
      uint32 pidx, nidx, tidx;
    };
    uint32 idxs[kNIdxs];
  };
};

class TriangleMesh {
 public:  
  static const size_t normal_size = sizeof(Normal);
  static const size_t vertex_size = sizeof(Point);
  static const size_t tcoord_size = 2 * sizeof(float);
  static const size_t idx_size = 3 * sizeof(uint32); // deprecated as it's misleading
  static const size_t tri_idx_size = 3 * sizeof(uint32);
 
  typedef std::vector<uint32> Idxs;
  typedef std::vector<float> VertAttribs;  

  ~TriangleMesh();  
  
  virtual void ApplyTransform(const ax::Matrix4x4 &m);

  void ScaleUV(float uscale, float vscale);

  AABB Bound() const;

  size_t vertices_size() const { return this->vertices_.size() * sizeof(float); }
  size_t normals_size() const { return this->normals_.size() * sizeof(float); }
  size_t indices_size() const { return this->indices_.size() * sizeof(uint32); }
  size_t adjacency_indces_size() const { return this->adj_indices_.size() * sizeof(uint32); }
  size_t tcoords_size() const { return this->tcoords_.size() * sizeof(float); }
  size_t n_triangles() const { return this->indices_.size() / 3; }
  size_t n_vertices() const { return this->vertices_.size() / 3; }

  const float* vertices() const { return &vertices_[0]; }
  const float *normals() const { return &normals_[0]; }
  const float *tcoords() const { return &tcoords_[0]; }
  const uint32 *indices() const { return &indices_[0]; }
  
  const float *vertex(uint32 i) const { return &vertices_[3*i]; }  

  const float *normal(uint32 i) const { return &normals_[3*i]; }  
  const float *tcoord(uint32 i) const { return &tcoords_[2*i]; }

  const uint32 *tri_indices(uint32 i) const { return &indices_[3*i]; }

  bool has_tcoord() const { return !this->tcoords_.empty(); }
  bool has_normal() const { return !this->normals_.empty(); } 
  bool has_adjacency() const { return !this->adj_indices_.empty(); }

  const uint32 *adj_indices() const { return &this->adj_indices_[0]; }  

  ax::Vector3 Vertex(int i) const { 
    const float *vptr = this->vertex(i);
    return ax::Vector3(vptr[0], vptr[1], vptr[2]);
  }

  void ComputeBound() const;
protected:
  TriangleMesh() { }

  float *vertex(uint32 i) { return &vertices_[3 * i]; }
  float *normal(uint32 i) { return &normals_[3 * i]; }

  void ReserveVertices(int n_vertices) {
    this->vertices_.reserve(n_vertices*3);
  }
  void ReserveNormals(int n_normals) {
    this->normals_.reserve(n_normals * 3);
  }
  void ReserveTcoords(int n_tcoords) {
    this->tcoords_.reserve(n_tcoords * 2);
  }

  void ReserveTriangles(int n_tris) { this->indices_.reserve(n_tris * 3); }

  void ReserveAdjacency() {
    this->adj_indices_.reserve(this->indices_.size() * 2);
  }

  void AddVertex(const float *vptr) {
    for (int i = 0; i < 3; ++i) this->vertices_.push_back(vptr[i]);
  }
  void AddNormal(const float *nptr) {
    for (int i = 0; i < 3; ++i) this->normals_.push_back(nptr[i]);
  }
  void AddTcoord(const float *tptr) {
    for (int i = 0; i < 3; ++i) this->tcoords_.push_back(tptr[i]);
  }
  void AddIndex(uint32 idx) {
    this->indices_.push_back(idx);
  }

  void SetNormals(const float *ptr, uint32 n) {
    if (n > 0) this->normals_ = VertAttribs(ptr, ptr + 3 * n);
  }
  void SetVertices(const float *ptr, uint32 n) {
    if (n > 0) this->vertices_ = VertAttribs(ptr, ptr + 3 * n);    
  }
  void SetTcoords(const float *ptr, uint32 n) {
    if (n > 0) this->tcoords_ = VertAttribs(ptr, ptr + 2 * n);
  }
  void SetIndices(const float *ptr, uint32 n_tri) {
    if (n_tri > 0) this->indices_ = Idxs(ptr, ptr + n_tri * 3);
  }
protected:
  VertAttribs vertices_;
  VertAttribs normals_;
  VertAttribs tcoords_;
  Idxs indices_;  

  Idxs adj_indices_;

private:
  mutable AABB bound_;
  mutable bool bound_need_update_;

  DISABLE_COPY_AND_ASSIGN(TriangleMesh);
};

class SphereTriMesh;
// TODO

//TriMeshPtr CreateSphere(float radius, uint32 slices, uint32 stacks);
//TriMeshPtr CreateCylinder(float base_radius, float top_radius, 
//                          float height, uint32 slices, uint32 stacks);
//TriMeshPtr CreateBoxMesh(const Point &pos1, const Point &pos2);
//DEPRECATED(TriMeshPtr LoadObj(const char *filename));

TriMeshPtr LoadObj(const std::string &filename);

void FinalizeModel(const Point *vertices, uint32 n_vertices, 
                   uint32 *indices, uint32 *face_count, 
                   Normal *normals);

typedef std::vector<const TriangleMesh*> MeshSet;

} // ax

#endif // AXLE_MODEL_TRIANGLEMESH_H
