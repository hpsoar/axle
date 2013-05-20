#ifndef AXLE_CG_MATERIAL_FACTORY_H
#define AXLE_CG_MATERIAL_FACTORY_H

#include "cg_fwd.h"

namespace ax {
class MaterialFactory {
public:
  static MaterialPtr CreateGLMaterial(const char *name, FILE *fp);
  static MaterialPtr CreateGLSLMaterial(const char *name, FILE *fp, Scene *s);
  static GLMaterialPtr CreateGLMaterial();
  static GLSLMaterialPtr CreateGLSLMaterial();
};
} // ax

#endif // AXLE_CG_MATERIAL_FACTORY_H

