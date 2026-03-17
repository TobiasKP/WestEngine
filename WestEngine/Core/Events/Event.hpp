#pragma once

#include "EventPayload.hpp"
#include "../../Constants/InternalEvents.hpp"

#include <functional>
#include <vector>

struct Event
{
  EventIdentifiers name;
  std::vector<std::function<void(EventIdentifiers, EventPayload)>> subscriber;
  EventPayload payload;
};
