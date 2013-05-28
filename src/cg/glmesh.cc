#include "glmesh.h"
#include "utils.h"
#include "gl_buffer.h"

namespace ax {
void GLGroup::Draw(ProgramGLSLPtr prog, Options opts) const {  
  this->material()->Enable(prog);
  this->Draw(opts);
  this->material()->Disable();
}

void GLGroup::Draw(const Scene *s, Options opts) const {  
  this->material()->Enable(s);
  this->Draw(opts);
  this->material()->Disable();
}

void GLGroup::Draw(Options opts) const {
  // NOTE: hard coded
  // TODO: solve relevant problems
  if (opts.Contain(ax::kNeedAdjacency)) {
    glDrawElements(GL_TRIANGLES_ADJACENCY_EXT,
                   this->n_tris_ * 6, GL_UNSIGNED_INT, 
                  (GLvoid*)((tri_start_)* 6 * sizeof(int)));
  }
  else {
    glDrawElements(GL_TRIANGLES, (n_tris_) * 3, GL_UNSIGNED_INT,
                  (GLvoid*)((tri_start_)* 3 * sizeof(int)));
  }
  CheckErrorsGL("GLGroup::Draw");
}

GLMesh::~GLMesh() { }

void GLMesh::PreProcess(Options opts) {
  if (opts.Contain(kComputeBound)) bound_ = mesh_->Bound();  
  if (opts.Contain(kUseVBO)) this->LoadToVBO(opts);
}

void GLMesh::LoadToVBO(Options opts) {
  size_t vert_buff_size = mesh_->vertices_size();
  if (mesh_->has_normal()) vert_buff_size += mesh_->normals_size();
  if (mesh_->has_tcoord()) vert_buff_size += mesh_->tcoords_size();
  
  RET(this->vert_buffer_.Resize(vert_buff_size));
  
  int offset = 0;
  offset += this->vert_buffer_.SetData(offset, mesh_->vertices_size(), mesh_->vertices());
  if (mesh_->has_normal()) {
    offset += this->vert_buffer_.SetData(offset, mesh_->normals_size(), mesh_->normals());
  }
  if (mesh_->has_tcoord()) {
    offset += this->vert_buffer_.SetData(offset, mesh_->tcoords_size(), mesh_->tcoords());
  }

  RET(this->idx_buffer_.Resize(this->mesh_->indices_size(), this->mesh_->indices()));

  if (opts.Contain(ax::kNeedAdjacency) && this->mesh_->has_adjacency()) {
    RET(this->adj_idx_buffer_.Resize(this->mesh_->adjacency_indces_size(), this->mesh_->adj_indices()));
  }
  
  vertex_slot_ = 0;
  normal_slot_ = 2;
  tcoord_slot_ = 1;

  CheckErrorsGL("GLMesh::LoadToVBO");
}

void GLMesh::Draw(ProgramGLSLPtr prog, Options opts) const {
  RET(this->BeginDraw(opts));

  this->material()->Enable(prog);
  
  for (ObjectList::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
    (*it)->Draw(prog, opts);

  this->material()->Disable();
  this->EndDraw(opts);
}

void GLMesh::Draw(const Scene *s, Options opts) const {
  RET(this->BeginDraw(opts));
  this->material()->Enable(s);
  for (ObjectList::const_iterator it = objs_.begin(); it != objs_.end(); ++it)
    (*it)->Draw(s, opts);
  this->material()->Disable();
  this->EndDraw(opts);
}

bool GLMesh::BeginDraw(Options opts) const {
  this->vert_buffer_.Bind();

  int offset = 0;
  glEnableVertexAttribArray(vertex_slot_);  
  glVertexAttribPointer(vertex_slot_, 3, GL_FLOAT, GL_FALSE, 0, 
                        (GLvoid*)offset);
  offset += mesh_->vertices_size();

  if (mesh_->has_normal() && opts.Contain(ax::kUseNormal)) {
    if (opts.Contain(kUseNormal)) {
      glEnableVertexAttribArray(normal_slot_);
      glVertexAttribPointer(normal_slot_, 3, GL_FLOAT, GL_FALSE, 0,
                            (GLvoid*)offset);
    }
    offset += mesh_->normals_size();
  }

  if (mesh_->has_tcoord() && opts.Contain(ax::kUseTexture)) {
    glEnableVertexAttribArray(tcoord_slot_);
    glVertexAttribPointer(tcoord_slot_, 2, GL_FLOAT, GL_FALSE, 0,
                          (GLvoid*)offset);
  }

  this->vert_buffer_.Unbind();

  if (opts.Contain(ax::kNeedAdjacency)) {
    if (this->adj_idx_buffer_.size() > 0) {
      this->adj_idx_buffer_.Bind();
    }
    else {
      ax::Logger::Log("[GLMesh::BeginDraw]No adjacency triangles available");
      ax::Logger::Log("[GLMesh::BeginDraw]Please make sure the model is loaded with appropriate flags");
      return false;
    }
  }
  else this->idx_buffer_.Bind();
  
  return !CheckErrorsGL("GLMesh::BeginDraw");
}

void GLMesh::EndDraw(Options opts) const {
  glDisableVertexAttribArray(vertex_slot_);

  glDisableVertexAttribArray(normal_slot_);
  glDisableVertexAttribArray(tcoord_slot_);

  this->idx_buffer_.Unbind();
  this->adj_idx_buffer_.Unbind();
  
  CheckErrorsGL("GLMesh::EndDraw");
}

ax::AABB GLMesh::ComputeBound(const ax::Matrix4x4 &m) const {
  ax::AABB box;
  for (size_t i = 0; i < mesh_->n_vertices(); ++i) {
    ax::Vector4 p = m * ax::Vector4(mesh_->Vertex(i), 1.f);
    p /= p.w;
    box.Union(ax::Point(p));
  }
  return box;
}

void GLMesh::ApplyTransform(const ax::Matrix4x4 &m) {
  mesh_->ApplyTransform(m * this->transform());
}

} // ax
