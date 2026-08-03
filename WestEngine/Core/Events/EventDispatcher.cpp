#include "EventDispatcher.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../Scripting/LuaFacade.hpp"

#include <format>

void EventDispatcher::init()
{
  LuaFacade::getLuaFacadeInstance().registerCFunction(event, LuaAPI::C_EVENT.data(), this);
}


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
  auto it = _nameToIdx.find(name);
  if (it == _nameToIdx.end())
  {
    _logger->log(
      Level::Error,
      std::format("Event not registered: {}, can not subscribe to it, make sure it was registered on startup!\n",
                  static_cast<std::int32_t>(name)));
    return;
  }
  std::int32_t idx = it->second;
  Event& e         = _events[idx];
  e.subscriber.push_back(callback);
};

int EventDispatcher::event(lua_State* L)
{
  EventDispatcher* me = (EventDispatcher*)lua_touserdata(L, lua_upvalueindex(1));
  std::int32_t state  = lua_tointeger(L, 1);
  GamePayload e       = {};
  e.turn              = state;
  me->dispatchEvent(EventIdentifiers::GAME_EVENT, std::move(e));
  return 0;
}
