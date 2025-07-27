#pragma once

#include <map>

#include "../Entity/Entity.h"
#include "../Utils/WestString.h"

class SystemFactory {
public:
  SystemFactory() {};
  void createSystem(std::map<const char *, std::int32_t, CStrCmp> infos,
                    const char *name, Entity *e);
};
