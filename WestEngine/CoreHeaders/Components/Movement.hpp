#pragma once

#include "../Interfaces/IComponent.h"

#include <atomic>
#include <vector>

enum class algorithm { MANHATTAN };

struct Movement : public IComponent
{
  algorithm a           = algorithm::MANHATTAN;
  std::int32_t range    = 0;
  glm::vec3 destination = glm::vec3(0);
  std::vector<std::int32_t> reachableTiles;
  std::atomic<bool> movementPending = false, moving = false;
};
