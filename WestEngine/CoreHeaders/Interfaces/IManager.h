#pragma once

#include <CoreConstants.hpp>
#include <cassert>
#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <TimeUtils.hpp>
#include <WestLogger.h>

class IManager
{
public:
  IManager(WestLogger* logger) : _logger(logger) {};
  virtual ~IManager() {};

  virtual std::int32_t startup() = 0;
  virtual void shutdown()        = 0;
  virtual void update()          = 0;
  virtual std::int32_t init()    = 0;

  inline void setName(std::string_view name)
  {
    this->_name = name;
  }

  inline const std::string getName()
  {
    return this->_name;
  }
  inline WestLogger* getLogger()
  {
    return this->_logger;
  }

protected:
  inline void logFailure(const std::string message)
  {
    _logger->log(Level::Error, message);
  }
  inline void logDebug(const std::string message)
  {
    _logger->log(Level::Info, message);
  };
  inline void logCycle(const std::string message)
  {
    _logger->log(Level::Cycle, message);
  }

private:
  WestLogger* _logger = nullptr;
  std::string _name   = CoreConstants::UNDEFINED_STRING.data();
};
