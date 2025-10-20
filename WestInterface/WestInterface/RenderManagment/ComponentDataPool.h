#pragma once

#include "ComponentData.h"

#include <cstdint>
#include <vector>

class ComponentDataPool {
public:
  ComponentDataPool();
  ~ComponentDataPool();

  std::uint32_t reserveNew(std::uint32_t size);
  bool deleteRange(std::uint32_t start, std::uint32_t end);

  ComponentData *getDataAtLocation(std::uint32_t location);

private:
  std::vector<ComponentData *> _poolingData;
};
