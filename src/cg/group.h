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

