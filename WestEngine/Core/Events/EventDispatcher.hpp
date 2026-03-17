#pragma once

#include "../../Constants/InternalEvents.hpp"
#include "Event.hpp"

#include <cstdint>
#include <flat_map>
#include <WestLogger.h>

class EventDispatcher
{
public:
  using Callback = std::function<void(EventIdentifiers, EventPayload)>;

  EventDispatcher() {};
  ~EventDispatcher() {};

  void registerNewEvent(const EventIdentifiers event);
  void dispatchEvent(const EventIdentifiers name, EventPayload payload);
  void subscribe(const EventIdentifiers name, Callback callback);

private:
  std::mutex _mutex;
  std::flat_map<EventIdentifiers, std::int32_t> _nameToIdx;
  std::vector<Event> _events;
  WestLogger* _logger = &WestLogger::getLoggerInstance();
};
