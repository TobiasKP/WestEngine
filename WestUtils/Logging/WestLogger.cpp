#include "../Include/WestLogger.h"

#include <iostream>

#include "../Include/TimeUtils.hpp"

WestLogger WestLogger::_loggerInstance;
std::mutex WestLogger::_errorMutex;
std::mutex WestLogger::_logMutex;
std::mutex WestLogger::_cycleMutex;

WestLogger &WestLogger::getLoggerInstance() {
  std::scoped_lock lck{_logMutex, _errorMutex, _cycleMutex};

  static WestLogger instance;
  return _loggerInstance;
}

WestLogger::WestLogger() {
  std::string currentDate = TimeUtils::getCurrentTimeAsDate() + ".log";
  _logFile.open(INFO_FILE_NAME + currentDate,
                std::ios::out | std::ios::app);
  _errorFile.open(ERROR_FILE_NAME + currentDate,
                  std::ios::out | std::ios::app);
  _cycleFile.open(CYCLE_FILE_NAME + currentDate, std::ios::out);
  _cycleLength = 100;

  if (!_logFile.is_open() || !_logFile.good()) {
    _logFile.close();
#ifdef DEBUG
    std::cerr << "Error opening log file - badbit|failbit|eofbit "
              << _logFile.bad() << _logFile.fail() << _logFile.eof()
              << std::endl;
#endif
  }
  if (!_errorFile.is_open() || !_errorFile.good()) {
    _errorFile.close();
    std::cerr << "Error opening error log file - badbit|failbit|eofbit "
              << _errorFile.bad() << _errorFile.fail() << _errorFile.eof()
              << std::endl;
  }
  if (!_cycleFile.is_open() || !_cycleFile.good()) {
    _cycleFile.close();
#ifdef DEBUG
    std::cerr << "Error opening cycling log file - badbit|failbit|eofbit "
              << _cycleFile.bad() << _cycleFile.fail() << _cycleFile.eof()
              << std::endl;
#endif
  }
}

WestLogger::~WestLogger() {}

void WestLogger::closeFileStreams() {
  writeInfo("Shutting down logging System\n");
  std::scoped_lock lck(_logMutex, _errorMutex, _cycleMutex);

  _cycleFile.close();
  _logFile.close();
  _errorFile.close();
}

void WestLogger::writeInfo(const std::string message) {
  std::lock_guard<std::mutex> lock(_logMutex);
  if (_logFile.is_open()) {
    _logFile << message;
    _logFile.flush();
  }
}

void WestLogger::writeError(const std::string message) {
  std::lock_guard<std::mutex> lock(_errorMutex);
  if (_errorFile.is_open()) {
    _errorFile << message;
    _errorFile.flush();
  } else {
    std::cerr << message << std::endl;
  }
}

void WestLogger::writeCycleLog(const std::string message) {
  std::lock_guard<std::mutex> lock(_cycleMutex);
  if (_cycleFile.is_open()) {
    _cycleFile << message;
    _cycleFile.flush();
  }
}

std::uint8_t WestLogger::getCycleLength() {
  if (_loggerInstance._cycleLength == 0) {
    _loggerInstance._cycleLength = 100;
    return 0;
  }
  return _loggerInstance._cycleLength--;
}
