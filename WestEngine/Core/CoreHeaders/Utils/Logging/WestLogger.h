#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "../../../Constants/CoreConstants.h"

class WestLogger {
public:
  static WestLogger &getLoggerInstance();

  WestLogger(WestLogger const &) = delete;
  void operator=(WestLogger const &) = delete;

  void writeInfo(const std::string message);
  void writeError(const std::string message);
  void closeFileStreams();

private:
  WestLogger();
  ~WestLogger();

  std::ofstream _logFile;
  std::ofstream _errorFile;
  static WestLogger _loggerInstance;
  static std::mutex _mutex;
};
