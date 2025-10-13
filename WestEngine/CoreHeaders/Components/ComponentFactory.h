#pragma once

#include <map>

#include "../Entity/Entity.h"

class ComponentFactory {
public:
  ComponentFactory() {};
  void createComponent(std::map<std::string, std::int32_t> infos,
                       std::string name, Entity *e);
};
