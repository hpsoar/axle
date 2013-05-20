#include "gl_object.h"
#include "utils.h"

namespace ax {
bool GLObject::CheckResult(const std::string &method) const {
  const std::string str = name_ + ", " + method;
  return !CheckErrorsGL(str.c_str());
}
}
