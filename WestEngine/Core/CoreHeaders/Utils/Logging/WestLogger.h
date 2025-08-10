#pragma once

#include <fstream>
#include <mutex>
#include <string>
#include <cstdint>

#include "../../../Constants/CoreConstants.h"

class WestLogger {
public:
  static WestLogger &getLoggerInstance();

  WestLogger(WestLogger const &) = delete;
  void operator=(WestLogger const &) = delete;

  void writeInfo(const std::string message);
  void writeError(const std::string message);
  void writeCycleLog(const std::string message);
  void closeFileStreams();

  static std::uint8_t getCycleLength(); 

private:
  WestLogger();
  ~WestLogger();

  std::ofstream _logFile;
  std::ofstream _errorFile;
  std::ofstream _cycleFile;
  std::uint8_t _cycleLength;
  static WestLogger _loggerInstance;
  static std::mutex _errorMutex;
  static std::mutex _logMutex;
  static std::mutex _cycleMutex;
};
