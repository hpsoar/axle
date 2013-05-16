#ifndef AXLE_CG_GLSLMATERIAL_H
#define AXLE_CG_GLSLMATERIAL_H

#include "../cg/material.h"
#include "../cg/cg_fwd.h"
#include "../geom.h"
#include <vector>

namespace ax {
class GLSLMaterial : public Material {
public:
  virtual void Enable(const Scene *s) const;
  virtual void Enable(ProgramGLSLPtr prog) const;
  virtual bool HasTexture() const;

protected:
  GLSLMaterial() { }

private:
  void BindConst(const char *name, float val) {
    bind_float_names_.push_back(name);
    bind_float_vals_.push_back(val);
  }
  void BindConstVec4(const char *name, const ax::Vector4 &val) {
    bind_const4_names_.push_back(name);
    bind_const4_vals_.push_back(val);
  }
  void BindConstVec3(const char *name, const ax::Vector3 &val) {
    bind_const3_names_.push_back(name);
    bind_const3_vals_.push_back(val);
  }
  void BindTexture(const char *name, const Texture2DPtr &tex) {
    bind_tex_names_.push_back(name);
    bind_texs_.push_back(tex);
  }
  void BindVertSubroutine(const char *name) {
    assert(vert_subroutine_.empty());
    vert_subroutine_ = name;
  }

  void BindFragSubroutine(const char *name) {
    assert(frag_subroutine_.empty());
    frag_subroutine_ = name;
  }

  template<typename N, typename V, typename F>
  void Bind(const N &names, const V &vals, F f) {
    for (size_t i = 0; i < names.size(); ++i) {
      f(names[i], vals[i]);
    }
  }

  typedef std::vector<std::string> VarNames;
  
  VarNames bind_const3_names_;
  std::vector<ax::Vector3> bind_const3_vals_;

  VarNames bind_const4_names_;
  std::vector<ax::Vector4> bind_const4_vals_;

  VarNames bind_float_names_;
  std::vector<float> bind_float_vals_;

  VarNames bind_tex_names_;  
  std::vector<Texture2DPtr> bind_texs_;

  std::string vert_subroutine_;
  std::string frag_subroutine_;

  friend class MaterialFactory;
};

} // ax

#endif // AXLE_CG_GLSLMATERIAL_H

