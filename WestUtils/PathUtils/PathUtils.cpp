#include "PathUtils.h"

#include <fcntl.h>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif

#include <filesystem>
#include <WestLogger.h>

namespace PathUtils
{

const std::string& getExecutableDir()
{
  static const std::string dir = []()
  {
#ifdef _WIN32
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    return std::filesystem::path(buffer).parent_path().string();
#else
    return std::filesystem::canonical("/proc/self/exe").parent_path().string();
#endif
  }();
  return dir;
}

std::string resolve(const std::string& relativePath)
{
  return getExecutableDir() + relativePath;
}


FILE* openFile(const std::string& path, bool readOnly)
{
  std::string filePath = resolve("/" + path);
  FILE* file = nullptr;

  if (readOnly)
  {
#ifdef _WIN32
    int fd;
    if (_sopen_s(&fd, filePath.c_str(), _O_RDONLY | _O_BINARY, _SH_DENYNO, 0) != 0 || fd == -1)
    {
      WestLogger::getLoggerInstance().log(Level::Error, std::format("---Error opening File!\n Path: {}\n", filePath));
      return nullptr;
    }
    if ((file = _fdopen(fd, "rb")) == NULL)
    {
      WestLogger::getLoggerInstance().log(Level::Error, "---Error opening File!\n");
      _close(fd);
      return nullptr;
    }
#else
    int fd;
    if ((fd = open(filePath.c_str(), O_RDONLY)) == -1)
    {
      WestLogger::getLoggerInstance().log(Level::Error, std::format("---Error opening File!\n Path: {}\n", filePath));
      return nullptr;
    }
    if ((file = fdopen(fd, "r")) == NULL)
    {
      WestLogger::getLoggerInstance().log(Level::Error, "---Error opening File!\n");
      return nullptr;
    }
#endif
  }
  else if ((file = fopen(filePath.c_str(), "w")) == NULL)
  {
    WestLogger::getLoggerInstance().log(Level::Error, "---Error opening File!\n");
    return nullptr;
  }

  return file;
}

}  // namespace PathUtils
