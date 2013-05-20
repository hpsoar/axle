#ifndef AXLE_CG_CG_OBJECT_H
#define AXLE_CG_CG_OBJECT_H

#include <GL/glew.h>
#include <string>
#include "../core/macros.h"

namespace ax {
class GLObject {
public:  
  virtual ~GLObject() { id_ = 0; }

  const std::string &name() const { return name_; }

  GLuint id() const { return id_; }

protected:
  GLObject(const std::string &name) : name_(name), id_(0) { }

  bool CheckResult(const std::string &method) const;

protected:
  GLuint id_;
  const std::string name_;

  DISABLE_COPY_AND_ASSIGN(GLObject);
};
} // ax

#endif // AXLE_CG_CG_OBJECT_H

