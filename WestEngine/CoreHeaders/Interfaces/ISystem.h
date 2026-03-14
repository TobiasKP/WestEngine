#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "../Components/ComponentRegistry.hpp"
#include "../Core/Scripting/LuaFacade.hpp"

#include <CoreConstants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <string_view>
#include <WestLogger.h>

class ISystem
{
public:
  ISystem() : _name(std::string(CoreConstants::UNDEFINED_STRING)) {};
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

  virtual void update()              = 0;
  virtual void updateDebuggingInfo() = 0;
  virtual void init()                = 0;

protected:
  ComponentRegistry* _reg;
  WestLogger* _logger;
  LuaFacade::LuaStates _state = LuaFacade::LuaStates::IDLE;

private:
  std::string _name = CoreConstants::UNDEFINED_STRING.data();
};
