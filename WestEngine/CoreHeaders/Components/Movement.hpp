#pragma once

#include "../Interfaces/IComponent.h"

#include <atomic>
#include <vector>

enum algorithm { MANHATTAN };

struct Movement : public IComponent
{
  algorithm a        = MANHATTAN;
  std::int32_t range = 0;
  std::vector<std::int32_t> reachableTiles;
  std::atomic<bool> movementPending = false;
};
