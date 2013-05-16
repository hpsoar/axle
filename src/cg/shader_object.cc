#include "../cg/shader_object.h"

#include <stdio.h>

#include "../core/debug.h"

namespace ax {

void ShaderObject::DeleteShader() {
  if(id_ > 0) {
    glDeleteShader(id_);
    id_ = 0;
  }
}

bool ShaderObject::Load(int type, const char *file) {
  assert(NULL != file);

  this->DeleteShader();

  id_ = glCreateShader(type);
  if (0 == id_) {
    this->CheckResult("ShaderObject::ShaderObject");
    return false;	
  }

  char *code = NULL;
  GLint len = ReadSahderFile(file, &code);
  if (0 == len) return false;
  glShaderSource(id_, 1, const_cast<const GLchar**>(&code), &len);
  delete[] code;

  glCompileShader(id_);
  return CheckCompileLog();
}

int ShaderObject::ReadSahderFile(const char *file, char** code) {
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
  }else {
    len = 0;
  }
  *code = buf;
  fclose(fp);

  return len;
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

