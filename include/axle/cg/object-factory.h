#ifndef AXLE_CG_OBJECT_FACTORY_H
#define AXLE_CG_OBJECT_FACTORY_H

#include "axle/cg/cg_fwd.h"

namespace ax {
class ObjectFactory {
public:
  //static ObjectPtr CreateGLMesh(FILE *fp, Scene *s);
  static ObjectPtr CreateGroup(FILE *fp, Scene *s);
  static ObjectPtr LoadWavefront(FILE *fp, Scene *s, Options opts);

  static GroupPtr CreateGroup();
private:
  static MaterialPtr LoadMaterial(const char *ptr, Scene *s, FILE *fp);
};
} // ax


#endif // AXLE_CG_OBJECT_FACTORY_H

