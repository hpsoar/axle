#ifndef AXLE_CG_OBJMESH_H
#define AXLE_CG_OBJMESH_H

#include "globject.h"
#include <vector>
#include "../model/triangle_mesh.h"
#include "cg_fwd.h"
#include "gl_buffer.h"

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
  void Draw(Options opts) const;
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
  uint32 vert_vbo() const { return this->vert_buffer_.id(); }
private:
  GLMesh(ax::TriMeshPtr mesh) : mesh_(mesh), idx_buffer_(GL_ELEMENT_ARRAY_BUFFER), adj_idx_buffer_(GL_ELEMENT_ARRAY_BUFFER) { }

  void BeginDraw(Options opts) const;
  void EndDraw(Options opts) const;
  void LoadToVBO(Options opts);
  void ComputeAdjacency();
private:
  typedef std::vector<GLGroupPtr> ObjectList;

  ObjectList objs_;
  ax::ArrayBufferGL vert_buffer_;
  ax::ArrayBufferGL idx_buffer_;
  ax::ArrayBufferGL adj_idx_buffer_;

  int vertex_slot_;
  int normal_slot_;
  int tcoord_slot_;

  ax::TriMeshPtr mesh_;
};

ObjectPtr LoadGLMesh(Scene *s, const std::string &filename, Options opts);

} // ax

#endif // AXLE_CG_OBJMESH_H
