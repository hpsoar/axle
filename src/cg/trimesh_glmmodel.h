#include "../model/triangle_mesh.h"
#include "cg_fwd.h"
#include "glm_avl.h"

namespace ax {
typedef std::tr1::shared_ptr<GLMmodel> GLMmodelPtr;
class TriMeshGLM : public ax::TriangleMesh {
public:
  static TriMeshPtr Create(GLMmodelPtr model, ax::Options opts) {
    ax::TriMeshPtr ptr = ax::TriMeshPtr(new TriMeshGLM);
    TriMeshGLM *ppp = dynamic_cast<TriMeshGLM*>(ptr.get());
    ppp->LoadFromGLMmodel(model, opts);
    return ptr;
  }
private:  
  TriMeshGLM() { }

  void LoadFromGLMmodel(GLMmodelPtr model, ax::Options opts);

  void Expand(GLMmodelPtr model);

  void GenAdjacency(GLMmodelPtr model);
};

}
