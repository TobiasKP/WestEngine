#pragma once

#include <fstream>
#include <mutex>

#include "../../../Constants/CoreConstants.h"

class WestLogger {
public:
  static WestLogger &getLoggerInstance();

  WestLogger(WestLogger const &) = delete;
  void operator=(WestLogger const &) = delete;

  void writeInfo(const char *message);
  void writeError(const char *message);
  void closeFileStreams();

private:
  WestLogger();
  ~WestLogger();

  std::ofstream _logFile;
  std::ofstream _errorFile;
  static WestLogger _loggerInstance;
  static std::mutex _mutex;
};
