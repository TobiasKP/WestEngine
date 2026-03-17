#pragma once

#include "EventPayload.hpp"
#include "../../Constants/InternalEvents.hpp"

#include <functional>
#include <string>
#include <vector>

struct Event
{
  std::string name;
  std::vector<std::function<void(EventIdentifiers, EventPayload)>> subscriber;
  EventPayload payload;
};
