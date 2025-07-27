#pragma once

#include <map>

#include "../Entity/Entity.h"
#include "../Utils/WestString.h"

class ComponentFactory {
public:
  ComponentFactory() {};
  void createComponent(std::map<const char *, std::int32_t, CStrCmp> infos,
                       const char *name, Entity *e);
};
