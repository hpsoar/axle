#include "axle/cg/glmaterial.h"
#include "axle/cg/utils.h"
#include <GL/glew.h>

namespace ax {
GLMaterial::GLMaterial(const char *name) : Material(name),
    ambient_(0.2f, 0.2f, 0.2f), diffuse_(0.8f, 0.8f, 0.8f),
    specular_(Colorf::kBlack), emission_(Colorf::kBlack), shininess_(0.65f) {
}

GLMaterial::GLMaterial(int predefined) : Material("") {
  // TODO
}

GLMaterial::GLMaterial(float *amb, float *dif, float *spec, float shiny,
                       const char *name) : Material(name) {
  // TODO
}

GLMaterial::GLMaterial(const Colorf &amb, const Colorf &dif, 
                       const Colorf &spec, float shiny, const char *name) :
    Material(name), ambient_(amb), diffuse_(dif), specular_(spec), 
    emission_(Colorf::kBlack), shininess_(shiny) {
}

void GLMaterial::Enable(const Scene *s) const{
  int which_face = GL_FRONT_AND_BACK;
  glMaterialfv(which_face, GL_AMBIENT, ambient_.ptr());
  glMaterialfv(which_face, GL_DIFFUSE, diffuse_.ptr());
  glMaterialfv(which_face, GL_SPECULAR, specular_.ptr());
  glMaterialfv(which_face, GL_EMISSION, emission_.ptr());
  glMaterialf(which_face, GL_SHININESS, shininess_);
  CheckErrorsGL("GLMaterial::Enable");
}

void GLMaterial::Enable(ProgramGLSLPtr prog) const {
  this->Enable((Scene*)0);
}

} // ax
