#include "../cg/glmesh.h"
#include "../cg/utils.h"

namespace ax {
void GLGroup::Draw(ProgramGLSLPtr prog, Options opts) const {  
  this->material()->Enable(prog);
  this->Draw();
  this->material()->Disable();
}

void GLGroup::Draw(const Scene *s, Options opts) const {  
  this->material()->Enable(s);
  this->Draw();
  this->material()->Disable();
}

void GLGroup::Draw() const {
  glDrawElements(GL_TRIANGLES, (n_tris_) * 3, GL_UNSIGNED_INT,
                (GLvoid*)((tri_start_)* 3 * sizeof(int)));
  CheckErrorsGL("GLGroup::Draw");
}

GLMesh::~GLMesh() { }

bool GLMesh::AllocateVertVBO(size_t size) {
  if (size > this->vert_vbo_size_) {
    glDeleteBuffers(1, &vert_vbo_);
    glGenBuffers(1, &vert_vbo_);

    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);

    V_RET(!CheckErrorsGL("AllocateVertVBO"));
    this->vert_vbo_size_ = size;
  } else {
    glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_);
  }

  return true;
}

bool GLMesh::AllocateIdxVBO(size_t size, const void *data) {
  if (size > this->idx_vbo_size_) {
    glDeleteBuffers(1, &idx_vbo_);
    glGenBuffers(1, &idx_vbo_);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    V_RET(!CheckErrorsGL("GLMesh::AllocateIdxVBO"));
    this->idx_vbo_size_ = size;
  } else {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo_);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);

    V_RET(!CheckErrorsGL("GLMesh::AllocateIdxVBO"));    
  }
  return true;
}

void GLMesh::PreProcess(Options opts) {
  if (opts.Contain(kComputeBound)) bound_ = mesh_->Bound();
  if (opts.Contain(kUseVBO)) this->LoadToVBO(opts);
}

void GLMesh::LoadToVBO(Options opts) {
  size_t vert_buff_size = mesh_->vertices_size();
  if (mesh_->has_normal()) vert_buff_size += mesh_->normals_size();
  if (mesh_->has_tcoord()) vert_buff_size += mesh_->tcoords_size();
  
  RET(this->AllocateVertVBO(vert_buff_size));
  
  int offset = 0;
  glBufferSubData(GL_ARRAY_BUFFER, offset, mesh_->vertices_size(),
                  mesh_->vertices());
  offset += mesh_->vertices_size();
  if (mesh_->has_normal()) {
    glBufferSubData(GL_ARRAY_BUFFER, offset, mesh_->normals_size(), 
                    mesh_->normals());
    offset += mesh_->normals_size();
  }
  if (mesh_->has_tcoord()) {
    glBufferSubData(GL_ARRAY_BUFFER, offset, mesh_->tcoords_size(), 
                    mesh_->tcoords());
  }

  RET(this->AllocateIdxVBO(mesh_->indices_size(), mesh_->indices()));
  
  vertex_slot_ = 0;
  normal_slot_ = 2;
  tcoord_slot_ = 1;

  CheckErrorsGL("GLMesh::LoadToVBO");
}

void GLMesh::Draw(ProgramGLSLPtr prog, Options opts) const {
  this->BeginDraw(opts);
  this->material()->Enable(prog);
  
  for (ObjectList::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
    (*it)->Draw(prog, opts);

  this->material()->Disable();
  this->EndDraw(opts);
}

void GLMesh::Draw(const Scene *s, Options opts) const {
  this->BeginDraw(opts);  
  this->material()->Enable(s);
  for (ObjectList::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
    (*it)->Draw(s, opts);
  this->material()->Disable();
  this->EndDraw(opts);
}

void GLMesh::BeginDraw(Options opts) const {
  glBindBuffer(GL_ARRAY_BUFFER, vert_vbo_);

  int offset = 0;
  glEnableVertexAttribArray(vertex_slot_);  
  glVertexAttribPointer(vertex_slot_, 3, GL_FLOAT, GL_FALSE, 0, 
                        (GLvoid*)offset);
  offset += mesh_->vertices_size();

  if (mesh_->has_normal()) {
    if (opts.Contain(kUseNormal)) {
      glEnableVertexAttribArray(normal_slot_);
      glVertexAttribPointer(normal_slot_, 3, GL_FLOAT, GL_FALSE, 0,
                            (GLvoid*)offset);
    }
    offset += mesh_->normals_size();
  }

  if (mesh_->has_tcoord() && opts.Contain(kUseTexture)) {
    glEnableVertexAttribArray(tcoord_slot_);
    glVertexAttribPointer(tcoord_slot_, 2, GL_FLOAT, GL_FALSE, 0,
                          (GLvoid*)offset);
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_vbo_);
  
  CheckErrorsGL("GLMesh::BeginDraw");
}

void GLMesh::EndDraw(Options opts) const {
  glDisableVertexAttribArray(vertex_slot_);

  glDisableVertexAttribArray(normal_slot_);
  glDisableVertexAttribArray(tcoord_slot_);

  CheckErrorsGL("GLMesh::EndDraw");
}

ax::AABB GLMesh::ComputeBound(const ax::Matrix4x4 &m) const {
  ax::AABB box;
  for (size_t i = 0; i < mesh_->n_vertices(); ++i) {
    ax::Vector4 p = m * ax::Vector4(mesh_->vertex(i), 1.f);
    p /= p.w;
    box.Union(ax::Point(p));
  }
  return box;
}

void GLMesh::ApplyTransform(const ax::Matrix4x4 &m) {
  mesh_->ApplyTransform(m * this->transform());
}

} // ax
