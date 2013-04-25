#include "axle/cg/program_glsl.h"

#include "axle/cg/shader_object.h"
#include "axle/cg/texture_gl.h"
#include <GL/glew.h>

namespace ax {
ProgramGLSLPtr ProgramGLSL::Create(const char *vertex_shader, 
                                   const char *fragment_shader,
                                   const std::string &name) {
  ProgramGLSLPtr ptr = ProgramGLSLPtr(new ProgramGLSL(name));
  if (NULL == ptr || !ptr->Load(vertex_shader, fragment_shader))
    return ProgramGLSLPtr();
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

bool ProgramGLSL::AttachShader(int type, const char *file) {  
  return this->AttachShader(ShaderObject::Create(type, file));
}

bool ProgramGLSL::Load(const char *vertex_shader, const char *fragment_shader) {
  if (NULL != vertex_shader) {
    if (!this->AttachShader(GL_VERTEX_SHADER, vertex_shader)) return false;
  }
  if (NULL != fragment_shader) {
    if (!this->AttachShader(GL_FRAGMENT_SHADER, fragment_shader)) return false;
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

void ProgramGLSL::SetSubroutineVar(const std::string &var_name, 
                                   const std::string &routine_name, 
                                   int shader_type) {
  GLuint ads = glGetSubroutineIndex(this->id(), shader_type, 
                                    routine_name.c_str());
  glUniformSubroutinesuiv(shader_type, 1, &ads);
}

} // ax
