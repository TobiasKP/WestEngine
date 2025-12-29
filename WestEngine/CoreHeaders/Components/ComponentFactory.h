#pragma once

#include "../Entity/Entity.h"

#include <map>

class ComponentFactory
{
public:
  ComponentFactory() {};
  void createComponent(std::map<std::string, std::int32_t> infos, std::string name, Entity& e);
};
