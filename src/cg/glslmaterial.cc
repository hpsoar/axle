#include "axle/cg/glslmaterial.h"
#include "axle/cg/program_glsl.h"

namespace ax {
void GLSLMaterial::Enable(const Scene *s) const {
  
}

bool GLSLMaterial::HasTexture() const {
  return bind_texs_.size() > 0;
}

void GLSLMaterial::Enable(ProgramGLSLPtr prog) const {  
   for (size_t i = 0; i < bind_float_names_.size(); ++i)
    prog->SetVar(bind_float_names_[i].c_str(), bind_float_vals_[i]);

  for (size_t i = 0; i < bind_const4_names_.size(); ++i)
    prog->Set4DVar(bind_const4_names_[i].c_str(), &bind_const4_vals_[i][0]);
  
  for (size_t i = 0; i < bind_const3_names_.size(); ++i) 
    prog->Set3DVar(bind_const3_names_[i].c_str(), &bind_const3_vals_[i][0]);
  
  for (size_t i = 0; i < bind_tex_names_.size(); ++i)
    prog->SetTextureVar(bind_tex_names_[i].c_str(), bind_texs_[i]);
 
  if (!vert_subroutine_.empty()) 
    prog->SetSubroutineVar("", vert_subroutine_, GL_VERTEX_SHADER);
  if (!frag_subroutine_.empty())
    prog->SetSubroutineVar("", frag_subroutine_, GL_FRAGMENT_SHADER);  

  CheckErrorsGL("GLSLMaterial::Enable");
}
} // ax

