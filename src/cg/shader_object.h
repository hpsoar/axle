#ifndef AXLE_GRAPHICS_SHADEROBJECT_H
#define AXLE_GRAPHICS_SHADEROBJECT_H

#include "axle/core/settings.h"
#include "axle/cg/cg_fwd.h"
#include "axle/cg/gl_object.h"

namespace ax {
class ShaderObject : public GLObject {
public:
  static ShaderPtr Create(int type, const char *file) {
    ShaderPtr ptr = ShaderPtr(new ShaderObject(file));
    if (NULL == ptr || !ptr->Load(type, file)) return ShaderPtr();
    return ptr;
  }

  ~ShaderObject() { this->DeleteShader(); }

private:
  static int ReadSahderFile(const char *file, char **code);

  ShaderObject(const std::string name = "") : GLObject(name) { };

  bool Load(int type, const char *file);

  void DeleteShader();

  bool CheckCompileLog();
  void PrintCompileLog(); 
};
} // ax

#endif // AXLE_GRAPHICS_SHADEROBJECT_H

