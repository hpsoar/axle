#ifndef AXLE_CG_GROUP_H
#define AXLE_CG_GROUP_H

#include "../cg/globject.h"
#include <vector>

namespace ax {
class Group : public Object {
public:  
  ~Group() { }

  void Add(ObjectPtr obj) { if (obj != NULL) objs_.push_back(obj); }

  void Draw(const Scene *s, Options opts) const;
  void Draw(ProgramGLSLPtr prog, Options opts) const;

  virtual ax::AABB ComputeBound(const ax::Matrix4x4 &m) const;
  virtual void PreProcess(Options opts);

  ObjectPtr object(int i) const { return this->objs_[i]; }

  uint32 n_objects() const { return this->objs_.size(); }

  uint32 n_primitives() const {
    uint32 n = 0;
    for (uint32 i = 0; i < this->n_objects(); ++i) n += this->object(i)->n_primitives();
    return n;
  }

  uint32 n_vertices() const {
    uint32 n = 0;
    for (uint32 i = 0; i < this->n_objects(); ++i) n += this->object(i)->n_vertices();
    return n;
  }
  
  virtual void GetVertices(ConstVertexSet &vertexet) const {
    for (uint32 i = 0; i < this->n_objects(); ++i) this->object(i)->GetVertices(vertexet);
  }
  virtual void GetIndices(ConstIndexSet &indexset) const {
    for (uint32 i = 0; i < this->n_objects(); ++i) this->object(i)->GetIndices(indexset);
  }
protected:
  Group() { }

  virtual void ApplyTransform(const ax::Matrix4x4 &m);

private:
  typedef std::vector<ObjectPtr> ObjectList;
  ObjectList objs_;

  friend class ObjectFactory;
};

} // ax

#endif // AXLE_CG_GROUP_H

