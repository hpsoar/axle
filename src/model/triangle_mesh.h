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

class TriangleMesh {
 public:  
  static const size_t normal_size = sizeof(Normal);
  static const size_t vertex_size = sizeof(Point);
  static const size_t tcoord_size = 2 * sizeof(float);
  static const size_t idx_size = 3 * sizeof(uint32); // deprecated as it's misleading
  static const size_t tri_idx_size = 3 * sizeof(uint32);
 
  typedef std::vector<uint32> Idxs;

  static TriMeshPtr Create(
      Point *vertices, Normal *normals, float *tcoords, uint32 *indices,
      size_t n_vertices, size_t n_triangles) {
    return TriMeshPtr(new TriangleMesh(vertices, normals, tcoords, indices,
                                       n_vertices, n_triangles));
  }
  ~TriangleMesh();  
  
  virtual void ApplyTransform(const ax::Matrix4x4 &m);

  void ScaleUV(float uscale, float vscale);

  AABB Bound() const;

  size_t vertices_size() const { return this->n_vertices() * this->vertex_size; }
  size_t normals_size() const { return this->n_vertices() * this->normal_size; }
  size_t indices_size() const { return this->n_triangles() * this->tri_idx_size; }
  size_t tcoords_size() const { return this->n_vertices() * this->tcoord_size; }
  size_t n_triangles() const { return n_triangles_; }
  size_t n_vertices() const { return n_vertices_; }

  const Point *vertices() const { return vertices_; }
  const Normal *normals() const { return normals_; }
  const float *tcoords() const { return tcoords_; }
  const uint32 *indices() const { return indices_; }
  
  const Point &vertex(int i) const { return vertices_[i]; }
  void set_vertex(int i, const Point &p) { vertices_[i] = p; }

  const Normal &normal(int i) const { return normals_[i]; }
  void set_normal(int i, const Normal &n) { normals_[i] = n; }
  float tcoord(int i) const { return tcoords_[i]; }
  int index(int i) const { return indices_[i]; }

  bool has_tcoord() const { return NULL != tcoords_; }
  bool has_normal() const { return NULL != normals_; }

  ITriangle GetITraingle(uint32 ti) const {
    assert(ti < this->n_triangles());
    return ITriangle(this->indices_ + 3 * ti);
  }

  void GenAdjacencyIndices();

  const Idxs &adj_indices() const { return this->adj_indices_; }
private:
  TriangleMesh(Point *vertices, Normal *normals, float *tcoords, 
               uint32 *indices, size_t n_vertices, size_t n_triangles)
      : vertices_(vertices), normals_(normals), tcoords_(tcoords),
        indices_(indices), n_vertices_(n_vertices),
        n_triangles_(n_triangles), bound_need_update_(true) {
  }

  void ComputeBound() const;
private:
  Point *vertices_;
  Normal *normals_;
  float *tcoords_;
  uint32 *indices_;
  size_t n_triangles_, n_vertices_;

  std::vector<uint32> adj_indices_;

  mutable AABB bound_;
  mutable bool bound_need_update_;

  DISABLE_COPY_AND_ASSIGN(TriangleMesh);
};

TriMeshPtr CreateSphere(float radius, uint32 slices, uint32 stacks);
TriMeshPtr CreateCylinder(float base_radius, float top_radius, 
                          float height, uint32 slices, uint32 stacks);
TriMeshPtr CreateBoxMesh(const Point &pos1, const Point &pos2);
TriMeshPtr LoadObj(const char *filename);

void FinalizeModel(const Point *vertices, uint32 n_vertices, 
                   uint32 *indices, uint32 *face_count, 
                   Normal *normals);
//void FinalizeModel(const Point *vertices, int n_vertices, int *indices,
//                   int *face_count, Normal *normals);
//void FirstPass(FILE* fd, int *n_vertices, int *normal_count, 
//               int *texcoord_count, int *face_count);
//void ParseIndices(char *line, int *vertex_idx, int *texcoord_idx, 
//                  int *normal_idx);
typedef std::vector<const TriangleMesh*> MeshSet;

} // ax

#endif // AXLE_MODEL_TRIANGLEMESH_H
