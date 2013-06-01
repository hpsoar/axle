#include "program_glsl.h"

#include "shader_object.h"
#include "texture_gl.h"
#include <GL/glew.h>

namespace ax {
ProgramGLSLPtr ProgramGLSL::Create(const char *vertex_shader, 
                                   const char *fragment_shader,
                                   const std::string &name,
                                   const MacroList &macros) {
  ProgramGLSLPtr ptr = ProgramGLSLPtr(new ProgramGLSL(name));
  if (NULL == ptr || !ptr->Load(vertex_shader, fragment_shader, macros))
    return ProgramGLSLPtr();
  return ptr;
}

ProgramGLSLPtr ProgramGLSL::CreateFromCode(const char *vert_src, const char *frag_src, const char *name) {
  ProgramGLSLPtr ptr = ProgramGLSLPtr(new ProgramGLSL(name));
  if (ptr == NULL ||
      !ptr->AttachShader(ax::ShaderObject::CreateFromCode(GL_VERTEX_SHADER, vert_src, name))||
      !ptr->AttachShader(ax::ShaderObject::CreateFromCode(GL_FRAGMENT_SHADER, frag_src, name))) return ProgramGLSLPtr();
  return ptr;
}

void ProgramGLSL::GeometryShaderSettings(GLenum input_type, 
                                         int max_emitted_verts,
                                         GLenum output_type) {
  glProgramParameteri(id_, GL_GEOMETRY_INPUT_TYPE_EXT, input_type);
  glProgramParameteri(id_, GL_GEOMETRY_OUTPUT_TYPE_EXT, output_type);
  glProgramParameteri(id_, GL_GEOMETRY_VERTICES_OUT_EXT, max_emitted_verts);

  this->CheckResult("ProgramGLSL::GeometryShaderSettings");
}

void ProgramGLSL::PrintLinkLog() const {
  GLint len = 0;	
  glGetProgramiv(id_, GL_INFO_LOG_LENGTH , &len);
  if(len <= 1) return;
  
  char *link_log = new char[len + 1];
  glGetProgramInfoLog(id_, len, &len, link_log);  
  link_log[len] = 0;

  Logger::Log("%s, ProgramGLSL::PrintLinkLog, %s", 
              this->name().c_str(), link_log);
  delete link_log;
}

bool ProgramGLSL::CheckLinkResult() {
  GLint status;
  glGetProgramiv(id_, GL_LINK_STATUS, &status);
  if (GL_TRUE == status) {
    is_linked_ = true;
  }
  else {
    PrintLinkLog();
  }
  return is_linked_;
}

bool ProgramGLSL::AttachShader(const ShaderPtr shader) const {
  if (NULL == shader) return false;
  glAttachShader(id_, shader->id());
  return true;
}

bool ProgramGLSL::AttachShader(int type, const char *file, const MacroList &macros) {
  return this->AttachShader(ShaderObject::Create(type, file, macros));
}

bool ProgramGLSL::Load(const char *vertex_shader, const char *fragment_shader, const MacroList &macros) {
  if (NULL != vertex_shader) {
    if (!this->AttachShader(GL_VERTEX_SHADER, vertex_shader, macros)) return false;
  }
  if (NULL != fragment_shader) {
    if (!this->AttachShader(GL_FRAGMENT_SHADER, fragment_shader, macros)) return false;
  }
  return true;
}

void ProgramGLSL::SetTextureVar(const char *name, 
                                const Texture2DPtr &tex) {
  this->SetTextureVar(name);
  tex->Bind();
}

void ProgramGLSL::SetTextureVar(const char *name, const Texture2DPtr *texs,
                                size_t n) {
  std::vector<int> vals(n);
  for (size_t i = 0; i < n; ++i) {
    int slot = this->NextTextureSlot();
    glActiveTexture(GL_TEXTURE0 + slot);
    texs[i]->Bind();
    vals[i] = slot;
  }
  this->Set1DVar(name, &vals[0], n);
}

void ProgramGLSL::SetImageVar(const char *name, ax::Texture2DPtr tex, uint32 access) {  
  int slot = this->NextImageSlot();
  tex->BindImageTexture(slot, access);  
  this->SetVar(name, tex->ImageUnit());
}

void ProgramGLSL::SetSubroutineVar(const std::string &var_name, 
                                   const std::string &routine_name, 
                                   int shader_type) {
  GLuint ads = glGetSubroutineIndex(this->id(), shader_type, 
                                    routine_name.c_str());
  glUniformSubroutinesuiv(shader_type, 1, &ads);
}

} // ax
