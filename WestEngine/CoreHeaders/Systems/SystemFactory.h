#pragma once

#include <map>

#include "../Entity/Entity.h"

class SystemFactory {
public:
  SystemFactory() {};
  void createSystem(std::map<std::string, std::int32_t> infos,
                    const std::string name, Entity *e);
};
