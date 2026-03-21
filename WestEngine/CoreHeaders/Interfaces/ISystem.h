#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "../Components/ComponentRegistry.hpp"
#include "../Constants/InternalEvents.hpp"
#include "../Core/Events/EventDispatcher.hpp"
#include "../Core/Scripting/LuaFacade.hpp"
#include "../Entity/World.hpp"

#include <CoreConstants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <string_view>
#include <ThreadSafeQueue.hpp>
#include <WestLogger.h>

class ISystem
{
public:
  ISystem() {};
  ISystem(std::shared_ptr<EventDispatcher> d, WestLogger* l, std::shared_ptr<ComponentRegistry> r)
    : _name(std::string(CoreConstants::UNDEFINED_STRING)), _dispatcher(d), _logger(l), _reg(r) {};
  virtual ~ISystem() {};

  // Getter
  inline std::string getName()
  {
    return _name;
  }
  // Setter
  inline void setName(std::string_view name)
  {
    _name = name;
  }
  inline void setState(LuaFacade::LuaStates s)
  {
    _state = s;
  }
  void pushEvent(EventIdentifiers event, EventPayload payload)
  {
    _eventQueue.push(std::make_tuple<>(event, payload));
  }

  virtual void update()                       = 0;
  virtual void updateDebuggingInfo()          = 0;
  virtual void init(const std::shared_ptr<World>& w) = 0;
  virtual void pollEvents() {};

protected:
  LuaFacade::LuaStates _state = LuaFacade::LuaStates::IDLE;
  tQueue<std::tuple<EventIdentifiers, EventPayload>> _eventQueue;
  std::shared_ptr<EventDispatcher> _dispatcher;
  std::shared_ptr<ComponentRegistry> _reg;

  WestLogger* _logger;

private:
  std::string _name = CoreConstants::UNDEFINED_STRING.data();
};
