#ifndef AXLE_CG_SHADEROBJECT_H
#define AXLE_CG_SHADEROBJECT_H

#include "../core/settings.h"
#include "cg_fwd.h"
#include "gl_object.h"

#include <list>
#include <hash_map>

namespace ax {
struct ShaderMacro {
  std::string name;
  std::string value;
};

typedef std::hash_map<std::string, std::string> MacroList;

class ShaderObject : public GLObject {
public:
  static ShaderPtr Create(int type, const char *file) {
    ShaderPtr ptr = ShaderPtr(new ShaderObject(file));
    if (NULL == ptr || !ptr->Load(type, file)) return ShaderPtr();
    return ptr;
  }

  static ShaderPtr Create(int type, const char *file, const MacroList &macros) {
    ShaderPtr ptr = ShaderPtr(new ShaderObject(file));
    if (NULL == ptr || !ptr->Load(type, file, macros)) return ShaderPtr();
    return ptr;
  }

  ~ShaderObject() { this->DeleteShader(); }

private:
  ShaderObject(const std::string name = "") : GLObject(name) { };

  bool Load(int type, const char *file, const MacroList &macros = MacroList());

  void DeleteShader();

  bool CheckCompileLog();
  void PrintCompileLog(); 
};
} // ax

#endif // AXLE_CG_SHADEROBJECT_H

