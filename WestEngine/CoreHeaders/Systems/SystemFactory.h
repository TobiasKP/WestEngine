#pragma once

#include "../Entity/Entity.h"

#include <map>

class SystemFactory
{
public:
  SystemFactory() {};
  void createSystem(std::map<std::string, std::int32_t> infos, const std::string name, const Entity& e);
};
