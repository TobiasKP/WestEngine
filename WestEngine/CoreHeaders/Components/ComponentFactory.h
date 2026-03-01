#pragma once

#include "../Entity/Entity.h"
#include "ComponentRegistry.hpp"

#include <map>

class ComponentFactory
{
public:
  ComponentFactory(ComponentRegistry* r) : _registry(r) {};
  void createComponent(std::map<std::string, std::int32_t> infos, std::string name, Entity& e);

private:
  ComponentRegistry* _registry;
};
