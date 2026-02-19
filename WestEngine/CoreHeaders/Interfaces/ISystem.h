#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "../Entity/Entity.h"
#include "../../Core/Scripting/LuaFacade.hpp"

#include <CoreConstants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

class ISystem
{
public:
  ISystem() : _name(CoreConstants::UNDEFINED_STRING) {};
  virtual ~ISystem() {};

  // Getter
  inline std::string getName()
  {
    return _name;
  }

  inline const std::vector<std::uint32_t>& getEntitieIds()
  {
    return _entitieIds;
  }

  // Setter
  inline void setName(const std::string name)
  {
    _name = name;
  }

  // Functions
  inline void addEntity(const Entity& e)
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _entitieIds.emplace_back(e.getId());
  }
  virtual void update()              = 0;
  virtual void updateDebuggingInfo() = 0;
  virtual void init()                = 0;

protected:
  std::mutex _mutex;
  LuaFacade* _facade;

private:
  std::string _name = CoreConstants::UNDEFINED_STRING;
  std::vector<std::uint32_t> _entitieIds;
};
