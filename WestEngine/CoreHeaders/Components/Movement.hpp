#pragma once

#include <deque>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

enum class algorithm { MANHATTAN };

struct Movement
{
  algorithm a           = algorithm::MANHATTAN;
  std::int32_t range    = 0;
  std::optional<glm::vec3> destination;
  std::deque<glm::vec3> path;
  std::vector<std::int32_t> reachableTiles;
#ifdef DEBUG
  bool debugInfoDisplayed = false, removeDebugInfo = false;
  std::uint32_t debugEntity = 0;
#endif
};
