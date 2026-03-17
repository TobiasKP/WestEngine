#pragma once

#include "../Constants/InternalEvents.hpp"
#include "../Core/Events/Event.hpp"

#include <cassert>
#include <CoreConstants.hpp>
#include <cstdint>
#include <string>
#include <string_view>
#include <ThreadSafeQueue.hpp>
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
  virtual void pollEvents() {};

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
  void pushEvent(EventIdentifiers event, EventPayload payload)
  {
    _eventQueue.push(std::make_tuple<>(event, payload));
  }

protected:
  tQueue<std::tuple<EventIdentifiers, EventPayload>> _eventQueue;
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
