#include "../core/io.h"

#if defined(SYS_IS_WINDOWS)
#include <windows.h>
#elif defined(SYS_IS_LINUX)
#include <dirent.h>
#endif

#include "../core/debug.h"
#include "../core/types.h"

namespace ax {
#if defined(SYS_IS_WINDOWS)
const int32 FileFilter::kDefaultAttr = FILE_ATTRIBUTE_NORMAL |
                                       FILE_ATTRIBUTE_READONLY |
                                       FILE_ATTRIBUTE_ARCHIVE;

#elif defined(SYS_IS_LINUX)
const int32 FileFilter::kDefaultAttr = DT_REG;

#endif

inline bool FileFilter::Pass(int32 file_attr,
                             const std::string &filename) const {
  if (0 == (file_attr & file_attr_))return false;
  if (exts_.length() > 0) {
    // file extention filter
    return false;
  }
  return true;
}

int GetFileNames(const std::string &dir, const FileFilter &filter,
                 std::vector<std::string> &filenames) {
  ax::Logger::Debug("GetFileNames, under folder: %s", dir.c_str());

#if defined(SYS_IS_WINDOWS)
  WIN32_FIND_DATA fileinfo;
  const std::string files = dir + "/*.*";

  HANDLE file = FindFirstFile(files.c_str(), &fileinfo);

  if (INVALID_HANDLE_VALUE == file) return kPathNotExist;
  
  DWORD error_code = S_OK;
  do {
    if (filter.Pass(fileinfo.dwFileAttributes, fileinfo.cFileName))
      filenames.push_back(dir + "/" + fileinfo.cFileName);
    FindNextFile(file, &fileinfo);
    error_code = GetLastError();
  } while (ERROR_NO_MORE_FILES != error_code);

#elif defined(SYS_IS_LINUX)
  DIR *dp = opendir(dir.c_str());
  if (NULL == dp) {
    ax::Logger::Debug("failed to open folder: %s", dir.c_str());
    return kPathNotExist;
  }

  struct dirent *dir_info = readdir(dp);
  while (NULL != dir_info) {
    if (filter.Pass(dir_info->d_type, dir_info->d_name)) {
      filenames.push_back(dir + "/" + dir_info->d_name);
    }
    dir_info = readdir(dp);
  }
  closedir(dp);
#endif

  ax::Logger::Debug("GetFileNames, %d files found.", filenames.size());
  return kOK;
}
} // ax

