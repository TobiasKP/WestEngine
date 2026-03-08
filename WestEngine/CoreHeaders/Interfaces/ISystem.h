#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "../../Core/Scripting/LuaFacade.hpp"
#include "../Components/ComponentRegistry.hpp"

#include <CoreConstants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <string_view>

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

  virtual void update()              = 0;
  virtual void updateDebuggingInfo() = 0;
  virtual void init()                = 0;

protected:
  LuaFacade* _facade;
  ComponentRegistry* _reg;

private:
  std::string _name = CoreConstants::UNDEFINED_STRING.data();
};
