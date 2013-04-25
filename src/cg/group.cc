#include "axle/cg/group.h"

namespace ax {
void Group::ApplyTransform(const ax::Matrix4x4 &m) {  
  for (ObjectList::iterator it = objs_.begin(); it != objs_.end(); ++it) {
    (*it)->Transform(m * this->transform());    
  }  
}
void Group::Draw(const Scene *s, Options opts) const {
  // TODO: crate a default null material, then we don't need the null test
  this->material()->Enable(s);
  
  ObjectList::const_iterator it = objs_.begin();
  for (; it != objs_.end(); ++it) (*it)->Draw(s, opts);
  
  this->material()->Disable();
}

void Group::Draw(ProgramGLSLPtr prog, Options opts) const {
  this->material()->Enable(prog);

  ObjectList::const_iterator it = objs_.begin();
  for (; it != objs_.end(); ++it) (*it)->Draw(prog, opts);
  
  this->material()->Disable();
}

void Group::PreProcess(Options opts) {
  for (ObjectList::iterator it = objs_.begin(); it != objs_.end(); ++it) {
    (*it)->PreProcess(opts);
  }
  if (opts.Contain(kComputeBound)) {
    ObjectList::const_iterator it = objs_.begin();
    for (; it != objs_.end(); ++it) bound_.Union((*it)->bound());
  }
}

ax::AABB Group::ComputeBound(const ax::Matrix4x4 &m) const {
  ax::AABB box;
  ObjectList::const_iterator it = objs_.begin();
  for (; it != objs_.end(); ++it) box.Union((*it)->ComputeBound(m));  
  return box;
}
} // ax
