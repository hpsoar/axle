#include "../cg/shader_object.h"

#include <stdio.h>

#include "../core/debug.h"

namespace ax {
std::string ReadSahderFile(const char *file) {
  FILE *fp = fopen(file, "rb");
  if (NULL == fp) {
    Logger::Log("'ShaderObject::ReadShaderFile', failed to open '%s'", file);
    return 0;
  }
  fseek(fp, 0, SEEK_END);
  int len = ftell(fp);
  rewind(fp);
  char *buf = NULL;
  if(len > 1) {
    buf = new char[len + 1];
    fread(buf, sizeof(char), len, fp);
    buf[len] = '\0';
  }  
  fclose(fp);

  return std::string(buf);
}

size_t FindNextMacro(const std::string &code, int s) {
  const std::string tag = "#define ";
  size_t p = code.find(tag, s);
  if (p == std::string::npos) return p;
  return p + tag.length();
}

void DefineMacros(std::string &code, const MacroList &macros) {
  if (macros.empty()) return;

  size_t p = 0;
  char macro[ax::kMaxLine];
  while ((p = ax::FindNextMacro(code, p)) != std::string::npos) {
    sscanf(code.c_str() + p, "%s", macro);
    auto mv = macros.find(macro);
    if (mv == macros.end()) continue;

    sprintf(macro + strlen(macro), " %s\n", mv->second.c_str()); // construct macro
    size_t n = code.find('\n', p);
    code.replace(p, n - p + 1, macro);
  }
}

void ShaderObject::DeleteShader() {
  if(id_ > 0) {
    glDeleteShader(id_);
    id_ = 0;
  }
}

bool ShaderObject::Load(int type, const char *file, const MacroList &macros) {
  assert(NULL != file);  
  
  std::string code = ReadSahderFile(file);
  if (code.length() < 10) return false; // NOTE: just be conservative!

  ax::DefineMacros(code, macros);

  const char *src = code.c_str();

  return this->LoadCode(type, code.c_str()); 
}

bool ShaderObject::LoadCode(int type, const char *code) {
  this->DeleteShader();

  id_ = glCreateShader(type);
  if (0 == id_) {
    this->CheckResult("ShaderObject::ShaderObject");
    return false;	
  }

  glShaderSource(id_, 1, const_cast<const GLchar**>(&code), NULL);  

  glCompileShader(id_);  

  return this->CheckCompileLog();
}

bool ShaderObject::CheckCompileLog() {
  GLint status;
  glGetShaderiv(id_, GL_COMPILE_STATUS, &status);
  if (GL_TRUE == status) return true;

  PrintCompileLog();
  return false;
}

void ShaderObject::PrintCompileLog() {
  GLint len = 0;	
  glGetShaderiv(id_, GL_INFO_LOG_LENGTH , &len);
  if(len < 2) return;
  char *compile_log = new char[len + 1];
  glGetShaderInfoLog(id_, len, &len, compile_log);
  Logger::Log("%s, ShaderObject::PrintCompileLog, %s", 
              this->name().c_str(), compile_log);
  delete[] compile_log;
}

} // ax

