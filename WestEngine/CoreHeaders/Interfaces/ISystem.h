#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>

#include "../Entity/Entity.h"
#include "../../Constants/CoreConstants.hpp"

class ISystem {
public:
  ISystem() : _name(CoreConstants::UNDEFINED_STRING) {};
  virtual ~ISystem() {};

  // Getter
  inline std::string getName() { return _name; }
  inline const std::vector<Entity *>& getEntities() { return _entities; }

  // Setter
  inline void setName(const std::string name) { _name = name; }

  // Functions
  inline void addEntity(Entity *e) {
    std::lock_guard<std::mutex> lock(_mutex);
    _entities.emplace_back(e);
  }
  virtual void update() = 0;
  virtual void updateDebuggingInfo() = 0;

protected:
  std::mutex _mutex;

private:
  std::string _name = CoreConstants::UNDEFINED_STRING;
  std::vector<Entity *> _entities;
};
