#ifndef AXLE_CG_GLOBJECT_H
#define AXLE_CG_GLOBJECT_H

#include "../core.h"
#include "../geom.h"
#include "cg_fwd.h"
#include "material.h"

namespace ax {
// should be handled with pointer only, 
// because idx_vbo_ may be deleted across copies

class Object {
public:    
  virtual ~Object() { }
 
  void set_material(const MaterialPtr &matl) {
    if (matl != NULL) matl_ = matl; 
  }
  void set_transform(const ax::Matrix4x4 &m) { transform_ = m; }
  void set_parent_group(const ObjectPtr &obj) { parent_group_ = obj; }

  const ax::Matrix4x4 &transform() const { return transform_; }
  const ax::MaterialPtr &material() const {
    assert(matl_ != NULL);
    return matl_; 
  }
  bool transform_applied() const { return transform_applied_; }  

  void Transform(const ax::Matrix4x4 &m = ax::Matrix4x4(1.f)) {
    if (this->transform_applied_) return;
    this->ApplyTransform(m);
    this->transform_applied_ = true;
  }
  
  const ax::AABB &bound() const { return bound_; }

  virtual void PreProcess(Options opts) = 0;

  virtual void Draw(const Scene *s, Options opts) const = 0;
  virtual void Draw(ProgramGLSLPtr prog, Options opts) const = 0;

  virtual ax::AABB ComputeBound(const ax::Matrix4x4 &m) const = 0;  
protected:
  Object() : transform_applied_(false), matl_(Material::kNullMatl) { }
  virtual void ApplyTransform(const ax::Matrix4x4 &m) = 0;

protected:
  ax::AABB bound_;
  MaterialPtr matl_;
  ax::Matrix4x4 transform_;
  ObjectPtr parent_group_;
  bool transform_applied_;    

  DISABLE_COPY_AND_ASSIGN(Object);
};

} // ax

#endif // AXLE_CG_GLOBJECT_H

