#ifndef AXLE_CG_GLMATERIAL_H
#define AXLE_CG_GLMATERIAL_H

#include "../cg/material.h"

namespace ax {
class GLMaterial : public Material {
public:  
  virtual void Enable(const Scene *s) const;
  virtual void Enable(ProgramGLSLPtr prog) const;  
      
  void set_ambient(const Colorf &c) { ambient_ = c; }
  void set_diffuse(const Colorf &c) { diffuse_ = c; }
  void set_specular(const Colorf &c) { specular_ = c; }
  void set_emission(const Colorf &c) { emission_ = c; }
  void set_shininess(float s) { shininess_ = s; }

  float shininess() const { return shininess_; }
  const Colorf &ambient() const { return ambient_; }
  const Colorf &diffuse() const { return diffuse_; }
  const Colorf &specular() const { return specular_; }
  const Colorf &emission() const { return emission_; }

protected:
  GLMaterial(const float *Kd, const float *Ka, const float *Ks, 
           const float shininess);

  GLMaterial(const char *name = s_unnamed);
  GLMaterial(int predefined);
  GLMaterial(float *amb, float *dif, float *spec, float shiny,
             const char *name = s_unnamed);
  GLMaterial(const Colorf &amb, const Colorf &dif, const Colorf &spec, 
             float shiny, const char *name = s_unnamed);

private:
  Colorf ambient_, diffuse_, specular_, emission_;
  float shininess_;

  friend class MaterialFactory;
};

} // ax

#endif 
