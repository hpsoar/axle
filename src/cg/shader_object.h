#ifndef AXLE_CG_SHADEROBJECT_H
#define AXLE_CG_SHADEROBJECT_H

#include "../core/settings.h"
#include "cg_fwd.h"
#include "gl_object.h"

#include <hash_map>

namespace ax {
typedef std::hash_map<std::string, std::string> MacroList;

class ShaderObject : public GLObject {
public:  
  static ShaderPtr Create(int type, const char *file, const MacroList &macros = MacroList()) {
    ShaderPtr ptr = ShaderPtr(new ShaderObject(file));
    if (NULL == ptr || !ptr->Load(type, file, macros)) return ShaderPtr();
    return ptr;
  }

  static ShaderPtr CreateFromCode(int type, const char *code, const char *name = "") {
    ShaderPtr ptr = ShaderPtr(new ShaderObject(name));
    if (ptr == NULL || !ptr->LoadCode(type, code)) return ShaderPtr();    
    return ptr;
  }

  ~ShaderObject() { this->DeleteShader(); }

private:
  ShaderObject(const std::string name = "") : GLObject(name) { };

  bool Load(int type, const char *file, const MacroList &macros = MacroList());

  bool LoadCode(int type, const char *code);

  void DeleteShader();

  bool CheckCompileLog();
  void PrintCompileLog(); 
};
} // ax

#endif // AXLE_CG_SHADEROBJECT_H

