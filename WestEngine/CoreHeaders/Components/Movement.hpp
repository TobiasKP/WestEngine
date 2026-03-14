#pragma once

#include <atomic>
#include <glm/glm.hpp>
#include <vector>

enum class algorithm { MANHATTAN };

struct Movement
{
  algorithm a           = algorithm::MANHATTAN;
  std::int32_t range    = 0;
  glm::vec3 destination = glm::vec3(0);
  std::vector<std::int32_t> reachableTiles;
#ifdef DEBUG
  bool debugInfoDisplayed = false, removeDebugInfo = false;
  std::uint32_t debugEntity = 0;
#endif
};
