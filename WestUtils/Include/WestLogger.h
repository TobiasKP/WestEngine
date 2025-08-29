#pragma once

#if defined(_WIN32) || defined(_WIN64)
#if defined(WESTUTILS_BUILDING_DLL)
#define WESTUTILS __declspec(dllexport)
#else
#define WESTUTILS __declspec(dllimport)
#endif
#else
#define WESTUTILS __attribute__((visibility("default")))
#endif

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>

const std::string INFO_FILE_NAME = "WestLog_";
const std::string ERROR_FILE_NAME = "WestError_";
const std::string CYCLE_FILE_NAME = "WestCyclingLog_";

class WESTUTILS WestLogger {
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
