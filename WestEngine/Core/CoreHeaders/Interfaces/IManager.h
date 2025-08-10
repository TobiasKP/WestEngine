#pragma once

#include <cassert>
#include <cstdint>
#include <string>
#include <format>
#include <iostream>

#include "../Utils/Logging/WestLogger.h"

class IManager {

public:
  IManager(WestLogger *logger) : _logger(logger) {};
  virtual ~IManager() {};

  virtual std::int32_t startup() = 0;
  virtual void shutdown() = 0;
  virtual void update() = 0;
  virtual std::int32_t init() = 0;

  inline void setName(std::string name) { this->_name = name; }

  inline const std::string getName() { return this->_name; }
  inline WestLogger *getLogger() { return this->_logger; }

protected:
  inline void logFailure(const std::string message) { _logger->writeError(message); }
  inline void logDebug(const std::string message) { _logger->writeInfo(message); };
  inline void logCycle(const std::string message) { _logger->writeCycleLog(message); }

private:
  WestLogger *_logger = nullptr;
  std::string _name = CoreConstants::UNDEFINED_STRING;
};

