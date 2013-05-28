#ifndef AXLE_CG_MATERIAL_H
#define AXLE_CG_MATERIAL_H

#include "../core.h"
#include "color.h"
#include "cg_fwd.h"

namespace ax {
class Material {
public:
  virtual ~Material() { }

  virtual void Enable(const Scene *s) const { }
  virtual void Enable(ProgramGLSLPtr prog) const { }
  virtual void Disable() const { }
  
  virtual Texture2DPtr texture() const { 
    ax::Logger::Log("Texture unsupported!"); 
    return Texture2DPtr();
  }

  virtual bool HasTexture() const { return false; }

  const std::string &name() const { return name_; }

  static const MaterialPtr kNullMatl;
protected:
  Material() : name_(s_unnamed) { }
  
  Material(const char *name) : name_(name) { }

  static const char *s_unnamed;
private:
  std::string name_;

  DISABLE_COPY_AND_ASSIGN(Material);
};

} // ax
#endif // AXLE_CG_MATERIAL_H
