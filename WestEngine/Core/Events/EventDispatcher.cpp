#include "EventDispatcher.hpp"

void EventDispatcher::registerNewEvent(const EventIdentifiers name)
{
  std::lock_guard<std::mutex> lock(_mutex);
  std::int32_t size = _events.size();
  _nameToIdx[name]  = size;
  Event e           = {};
  e.name            = name;
  _events.push_back(std::move(e));
};

void EventDispatcher::dispatchEvent(const EventIdentifiers name, EventPayload payload)
{
  std::lock_guard<std::mutex> lock(_mutex);
  std::int32_t idx            = _nameToIdx[name];
  std::vector<Callback>& subs = _events[idx].subscriber;
  for (Callback& sub : subs)
  {
    sub(name, payload);
  }
};

void EventDispatcher::subscribe(const EventIdentifiers name, Callback callback)
{
  std::lock_guard<std::mutex> lock(_mutex);
  std::int32_t idx = _nameToIdx[name];
  Event& e         = _events[idx];
  e.subscriber.push_back(callback);
};
