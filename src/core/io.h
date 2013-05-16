#ifndef AXLE_CORE_IO_H
#define AXLE_CORE_IO_H

#include "../core/settings.h"
#include "../core/types.h"
#include <vector>

namespace ax {
enum FileTypeFilter {
  kNormal = 0,
  kReadOnly = 0x00000001,
};

class FileFilter {
public:
  FileFilter() : file_attr_(kDefaultAttr) { }
  FileFilter(const std::string &exts) 
      : file_attr_(kDefaultAttr), exts_(exts) { }
  FileFilter(int32 file_attr) : file_attr_(file_attr) { }  
  FileFilter(int32 file_attr, const std::string &exts)
      : file_attr_(file_attr), exts_(exts) { }

private:
  bool Pass(int32 file_attr, const std::string &filename) const;

private:
  int32 file_attr_;
  std::string exts_;
  static const int32 kDefaultAttr;

  friend int GetFileNames(const std::string &dir, const FileFilter &filter,
                          std::vector<std::string> &filenames);
};

int GetFileNames(const std::string &dir, const FileFilter &filter, 
                 std::vector<std::string> &filenames);

inline int GetFileNames(const std::string &dir, 
                        std::vector<std::string> &filenames) {
  return GetFileNames(dir, FileFilter(), filenames);
}

} // ax

#endif // AXLE_CORE_IO_H
