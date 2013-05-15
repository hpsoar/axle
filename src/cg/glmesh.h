#ifndef AXLE_CG_OBJMESH_H
#define AXLE_CG_OBJMESH_H

#include "axle/cg/globject.h"
#include <vector>
#include "axle/model/triangle_mesh.h"
#include "axle/cg/cg_fwd.h"

namespace ax {
class GLGroup : public Object {
public:
  static GLGroupPtr Create(int tri_start, int n_tris) {
    return GLGroupPtr(new GLGroup(tri_start, n_tris));
  }
  virtual void Draw(const Scene *s, Options opts) const;
  virtual void Draw(ProgramGLSLPtr prog, Options opts) const;
private:  
  virtual void PreProcess(Options opts) { }  

  virtual ax::AABB ComputeBound(const ax::Matrix4x4 &m) const { 
    return ax::AABB(); 
  }
  virtual void ApplyTransform(const ax::Matrix4x4 &m) { }

  GLGroup(int tri_start, int n_tris) : 
      tri_start_(tri_start), n_tris_(n_tris) { }
private:
  void Draw() const;
private:
  int tri_start_;
  int n_tris_;
};

class GLMesh : public Object {
public:  
  static GLMeshPtr Create(ax::TriMeshPtr mesh, bool as_whole = false) {
    GLMeshPtr ptr(new GLMesh(mesh));
    if (as_whole) ptr->Add(GLGroup::Create(0, mesh->n_triangles()));
    return ptr;
  }  
  virtual void PreProcess(Options opts);
  virtual void Draw(const Scene *s, Options opts) const;
  virtual void Draw(ProgramGLSLPtr prog, Options opts) const;

  virtual ax::AABB ComputeBound(const ax::Matrix4x4 &m) const;
  virtual void ApplyTransform(const ax::Matrix4x4 &m);

  ~GLMesh();

  void Add(GLGroupPtr g) { objs_.push_back(g); }

  ax::TriMeshPtr tri_mesh() const { return this->mesh_; }
  uint32 vert_vbo() const { return vert_vbo_; }
private:
  GLMesh(ax::TriMeshPtr mesh) : 
      mesh_(mesh), idx_vbo_size_(0), vert_vbo_size_(0), 
      idx_vbo_(0), vert_vbo_(0) { }  

  void BeginDraw(Options opts) const;
  void EndDraw(Options opts) const;
  bool AllocateIdxVBO(size_t size, const void *data);
  bool AllocateVertVBO(size_t size);
  void LoadToVBO(Options opts);

private:
  typedef std::vector<GLGroupPtr> ObjectList;
  ObjectList objs_;
  uint32 vert_vbo_;
  uint32 idx_vbo_;
  int vertex_slot_;
  int normal_slot_;
  int tcoord_slot_;
  size_t idx_vbo_size_;
  size_t vert_vbo_size_;
  ax::TriMeshPtr mesh_;  
};

ObjectPtr LoadGLMesh(Scene *s, const std::string &filename, Options opts);

} // ax

#endif // AXLE_CG_OBJMESH_H
