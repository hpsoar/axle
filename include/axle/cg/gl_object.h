#ifndef AXLE_CG_CG_OBJECT_H
#define AXLE_CG_CG_OBJECT_H

#include "axle/cg/utils.h"
#include <GL/glew.h>

namespace ax {
class GLObject {
public:  
  virtual ~GLObject() { id_ = 0; }

  const std::string &name() const { return name_; }

  GLuint id() const { return id_; }

protected:
  GLObject(const std::string &name) : name_(name), id_(0) { }

  bool CheckResult(const std::string &method) const {
    const std::string str = name_ + ", " + method;
    return !CheckErrorsGL(str.c_str());
  }

protected:
  GLuint id_;
  const std::string name_;
};
} // ax

#endif // AXLE_CG_CG_OBJECT_H

