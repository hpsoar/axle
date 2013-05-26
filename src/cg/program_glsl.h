#ifndef AXLE_CG_PROGRAM_GLSL_H
#define AXLE_CG_PROGRAM_GLSL_H

#include "../core.h"
#include "cg_fwd.h"
#include "gl_object.h"
#include "shader_object.h"


#include <vector>

namespace ax {
class ProgramGLSL : public GLObject {
public:
  static ProgramGLSLPtr Create(const char *vertex_shader, 
                               const char *fragment_shader,
                               const std::string &name = "",
                               const MacroList &macros = MacroList());

  static ProgramGLSLPtr CreateFromCode(const char *vert_src, const char *frag_src, const char *name = "");

  virtual ~ProgramGLSL() { this->DeleteProgram(); }

  void Begin() { glUseProgram(id_); }
  void End() {
     if (this->texture_slot_ >= 0) this->ResetTextureSlot();
    glUseProgram(0); 
  }

  bool Load(const char *vertex_shader, const char *fragment_shader, const MacroList &macros = MacroList());

  bool AttachShader(const ShaderPtr shader) const;

  bool AttachShader(int type, const char *file, const MacroList &macros = MacroList());

  bool AttachFragShader(const char *file) { 
    return this->AttachShader(GL_FRAGMENT_SHADER, file);
  }
  bool AttachGeomShader(const char *file) {
    return this->AttachShader(GL_GEOMETRY_SHADER, file);
  }
  bool AttachVertShader(const char *file) {
    return this->AttachShader(GL_VERTEX_SHADER, file);
  }
  void GeometryShaderSettings(GLenum input_type, int max_emitted_verts, 
                              GLenum output_type);
  bool Link() {
    glLinkProgram(id_);
    return CheckLinkResult();
  }

  int GetUniformVar(const char *name) const {
    return glGetUniformLocation(id_, name);     
  }

  bool IsValid() const { return 0 != id_ && is_linked_; }

  void SetParameteri(uint32 name, int param) {
    glProgramParameteri(this->id(), name, param);
  }

  template<typename T>
  void SetVar(const char *name, T val) const {
    //// TODO: call SetScalarVar
    //GLint var = this->GetUniformVar(name);
    //if (this->IsVarValid(var, name)) glUniform1i(var, val);
    this->SetScalarVar(name, val, glUniform1i);
  }
  
  void SetVar(const char *name, float val) const {
    this->SetScalarVar(name, val, glUniform1f);
  }

  void SetVar(const char *name, GLuint64EXT val) const {
    this->SetScalarVar(name, val, glUniformui64NV);
  }

  void Set1DVar(const char *name, const GLuint64EXT *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniformui64vNV);
  }

  void Set1DVar(const char *name, const float *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform1fv);
  }

  void Set1DVar(const char *name, const int *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform1iv);
  }

  void Set2DVar(const char *name, const float *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform2fv);
  }

  void Set2DVar(const char *name, const int *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform2iv);
  }

  void Set3DVar(const char *name, const int *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform3iv);
  }

  void Set3DVar(const char *name, const float *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform3fv);
  }

  template<typename VEC3D>
  void Set3DVar(const char *name, const VEC3D &vec) {
    this->Set3DVar(name, &vec[0], 1);
  }

  void Set4DVar(const char *name, const int *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform4iv);
  }

  void Set4DVar(const char *name, const float *vals, int n = 1) const {
    this->SetArrayVar(name, n, vals, glUniform4fv);
  }

  template<typename MAT4D>
  void Set4DMatVar(const char *name, const MAT4D &m,
                   int transpose = GL_FALSE) const {
    this->Set4DMatVar(name, &m[0][0], 1, transpose);
  }

  void Set4DMatVar(const char *name, const float *vals, int n = 1, 
                   int transpose = GL_FALSE) const {
    this->SetMatArrayVar(name, n, transpose, vals, glUniformMatrix4fv);
  }

  template<typename MAT3D>
  void Set3DMatVar(const char *name, const MAT3D &m,
                   int transpose = GL_FALSE) const {
    this->Set3DMatVar(name, &m[0][0], 1, transpose);
  }

  void Set3DMatVar(const char *name, const float *vals, int n = 1,
                   int transpose = GL_FALSE) const {
    this->SetMatArrayVar(name, n, transpose, vals, glUniformMatrix3fv);
  }

  void SetTextureVar(const char *name, const Texture2DPtr &tex);
  
  void SetTextureVar(const char *name, const Texture2DPtr *texs, size_t n);

  void SetTextureVar(const char *name, const std::vector<Texture2DPtr> &texs) {
    this->SetTextureVar(name, &texs[0], texs.size());
  }
  
  void SetTextureVars(const char *names[], const Texture2DPtr *texs, 
                      size_t n) {
    for (size_t i = 0; i < n; ++i) this->SetTextureVar(names[i], texs[i]);
  }

  void SetTextureVars(const char *names[], 
                      const std::vector<Texture2DPtr> &texs) {
    this->SetTextureVars(names, &texs[0], texs.size());      
  }
  
  void SetTextureVar(const char *name) {
    int slot = this->NextTextureSlot(); 
    glActiveTexture(GL_TEXTURE0 + slot);  
    this->SetVar(name, slot);
  }

  void SetSubroutineVar(const std::string &var_name, 
                        const std::string &routine_name,
                        int shader_type);

private:
  ProgramGLSL(const std::string &name) : 
       GLObject(name), is_linked_(false), texture_slot_(-1) {
    id_ = glCreateProgram();
    if (0 == id_) this->CheckResult("ProgramGLSL::ProgramGLSL");
  }

  void DeleteProgram() {
    if (id_ > 0) glDeleteProgram(id_);    
  }  
  
  template<typename T, typename F>
  void SetScalarVar(const char *name, T val, F f) const {
    GLint var = this->GetUniformVar(name);
    if (this->IsVarValid(var, name)) f(var, val);
  }

  // TODO: rename to SetVectorVar
  template<typename T, typename F>
  void SetArrayVar(const char *name, int n, const T *vals, F f) const {
    GLint var = this->GetUniformVar(name);
    if (this->IsVarValid(var, name)) f(var, n, vals);
  }
  
  template<typename T, typename F>
  void SetMatArrayVar(const char *name, int n, int transpose, 
                    const T *vals, F f) const {
    GLint var = this->GetUniformVar(name);    
    if (this->IsVarValid(var, name)) f(var, n, transpose, vals);
  }

  bool IsVarValid(GLint var, const char *name = "") const {
    if (var < 0) {
      Logger::Debug("%s not exists in %s", name, this->name().c_str());
      return false;
    }
    return true;
  }

  void PrintLinkLog() const;
  bool CheckLinkResult();
  
  void ResetTextureSlot() {
    this->texture_slot_ = -1;
    glActiveTexture(GL_TEXTURE0);
  }
  int NextTextureSlot() {
    return ++this->texture_slot_;
  }

 private:  
  bool is_linked_;
  int texture_slot_;
};

inline bool InvalidVar(const int var) { return var < 0; }

} // ax

#endif // AXLE_CG_PROGRAM_GLSL_H
