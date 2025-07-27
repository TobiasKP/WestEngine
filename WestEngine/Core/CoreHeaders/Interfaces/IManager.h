#pragma once

#include <cassert>
#include <cstdint>
#include <iostream>

#include "../Utils/Logging/WestLogger.h"
#include "../Utils/WestString.h"

class IManager {

public:
  IManager(WestLogger *logger) : _logger(logger), _string(new WestString()) {};
  virtual ~IManager() {};

  virtual std::int32_t startup() = 0;
  virtual void shutdown() = 0;
  virtual void update() = 0;
  virtual std::int32_t init() = 0;

  inline void setName(const char *name) { this->_name = name; }

  inline const char *getName() { return this->_name; }
  inline WestString *getString() { return this->_string; }
  inline WestLogger *getLogger() { return this->_logger; }

protected:
  inline void logFailure(const char *message) { _logger->writeError(message); }
  inline void logDebug(const char *message) { _logger->writeInfo(message); };

private:
  WestLogger *_logger = nullptr;
  const char *_name = nullptr;
  WestString *_string = nullptr;
};

