#include "../core/options.h"
#include <stdio.h>
#if defined(SYS_IS_WINDOWS)
#include <windows.h>
#endif
#include "../core/types.h"

namespace ax {
int SysOptions::s_cores_num_ = 1;
bool SysOptions::s_initialized_ = false;

std::string UserOptions::s_default_media_path_;
bool UserOptions::s_initialized_ = false;

std::string UserOptions::GetFullModelPath(const char *filename) {  
  assert(initialized());
  char buff[kMaxPath];
  sprintf(buff, "%s/models/%s", s_default_media_path_.c_str(), filename);
  return buff;
}

std::string UserOptions::GetFullTexturePath(const char *filename) {
  assert(initialized());
  char buff[kMaxPath];
  sprintf(buff, "%s/textures/%s", s_default_media_path_.c_str(), filename);
  return buff;
}

std::string UserOptions::GetFullEnvMapPath(const char *filename) {
  assert(initialized());
  char buff[kMaxPath];
  sprintf(buff, "%s/env_maps/%s", s_default_media_path_.c_str(), filename);
  return buff;
}

void UserOptions::set_default_media_path(const char *path) {
  s_default_media_path_.assign(path);
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

void LoadUserOptions() {
  if (UserOptions::initialized()) {
    //warning for reinitializing
    return;
  }
  // load & parse file

  // if not specified in file
#if defined(SYS_IS_WINDOWS)
  UserOptions::set_default_media_path("E:/workspace/media/");
#elif defined(SYS_IS_LINUX)
  UserOptions::set_default_media_path("/media/E/workspace/media/");
#endif
  UserOptions::finalize();
}

void InitOptions() {
  InitSysOptions();
  LoadUserOptions();
}
} // ax
