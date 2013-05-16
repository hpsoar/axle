#include "options.h"
#include <stdio.h>
#if defined(SYS_IS_WINDOWS)
#include <windows.h>
#endif
#include "types.h"
#include "debug.h"

namespace ax {
int SysOptions::s_cores_num_ = 1;
bool SysOptions::s_initialized_ = false;

std::string UserOptions::s_default_media_path_;
bool UserOptions::s_initialized_ = false;

std::string UserOptions::GetFullModelPath(const char *filename) {    
  char buff[kMaxPath];
  sprintf(buff, "models/%s", filename);
  return UserOptions::GetFullPath(buff);
}

std::string UserOptions::GetFullTexturePath(const char *filename) {  
  char buff[kMaxPath];
  sprintf(buff, "textures/%s", filename);
  return UserOptions::GetFullPath(buff);
}

std::string UserOptions::GetFullEnvMapPath(const char *filename) {  
  char buff[kMaxPath];
  sprintf(buff, "env_maps/%s", filename);
  return UserOptions::GetFullPath(buff);
}

std::string UserOptions::GetFullPath(const char *filename) {
  if (!UserOptions::s_initialized_) UserOptions::LoadDefault();
  assert(UserOptions::s_initialized_);
  return UserOptions::s_default_media_path_ + "/" + filename;
}

void UserOptions::LoadDefault() {
  UserOptions::s_default_media_path_ = kDefaultMediaPath;
  UserOptions::s_initialized_ = true;
}

void InitSysOptions() {
  if (SysOptions::s_initialized_) {
    //warning for reinitializing
    return;
  }
#if defined(SYS_IS_WINDOWS)
  SYSTEM_INFO sys_info;
  GetSystemInfo(&sys_info);
  SysOptions::s_cores_num_ = sys_info.dwNumberOfProcessors;
#elif defined(SYS_IS_LINUX)
  SysOptions::s_cores_num_ = sysconf(_SC_NPROCESSORS_ONLN);
#endif
  SysOptions::s_initialized_ = true;
}

void InitOptions() {
  InitSysOptions();  
}

void PathList::AddPath(const char *path) {
  this->path_list_.push_back(path);
}

std::string PathList::GetFullPath(const char *filename) {
  for (auto path : this->path_list_) {
    auto fullname = path + filename;
    auto fp = fopen(fullname.c_str(), "r");
    if (fp != NULL) return fullname;
  }
  ax::Logger::Log("file: %s, not found", filename);
  return "";
}

FILE *PathList::OpenFile(const char *filename) {
  for (auto path: this->path_list_) {
    auto fullname = path + filename;
    auto fp = fopen(fullname.c_str(), "r");
    if (fp != NULL) return fp;
  }
  return nullptr;
}
} // ax
