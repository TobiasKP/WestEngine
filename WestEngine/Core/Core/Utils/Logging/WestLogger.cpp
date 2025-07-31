#include "../../../CoreHeaders/Utils/Logging/WestLogger.h"

#include <iostream>

WestLogger WestLogger::_loggerInstance;
std::mutex WestLogger::_mutex;

WestLogger &WestLogger::getLoggerInstance() {
  std::lock_guard<std::mutex> lock(_mutex);

  static WestLogger instance;
  return _loggerInstance;
}

WestLogger::WestLogger() {
  _logFile.open(CoreConstants::INFO_FILE_NAME, std::ios::out);
  _errorFile.open(CoreConstants::ERROR_FILE_NAME,
                  std::ios::out | std::ios::app);
  if (!_logFile.is_open() || !_logFile.good())
    _logFile.close();
  if (!_errorFile.is_open() || !_errorFile.good())
    _errorFile.close();
}

WestLogger::~WestLogger() { this->closeFileStreams(); }

void WestLogger::closeFileStreams() {
  writeInfo("Shutting down System");
  std::lock_guard<std::mutex> lock(_mutex);
  _logFile.close();
  _errorFile.close();
}

void WestLogger::writeInfo(const std::string message) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (_logFile.is_open()) {
    _logFile << message;
    _logFile.flush();
  }
}

void WestLogger::writeError(const std::string message) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (_errorFile.is_open()) {
    _errorFile << message;
    _errorFile.flush();
  }
}
