#pragma once

#include <glm/glm.hpp>
#include <atomic>
#include <vector>

enum class algorithm { MANHATTAN };

struct Movement
{
  algorithm a           = algorithm::MANHATTAN;
  std::int32_t range    = 0;
  glm::vec3 destination = glm::vec3(0);
  std::vector<std::int32_t> reachableTiles;
  std::atomic<bool> movementPending{false}, moving{false};
#ifdef DEBUG
  bool debugInfoDisplayed = false, removeDebugInfo = false;
  std::uint32_t debugEntity = 0;
#endif

  Movement() = default;
  Movement(Movement&& o) noexcept
      : a(o.a), range(o.range), destination(o.destination), reachableTiles(std::move(o.reachableTiles)),
        movementPending(o.movementPending.load()), moving(o.moving.load())
#ifdef DEBUG
        , debugInfoDisplayed(o.debugInfoDisplayed), removeDebugInfo(o.removeDebugInfo), debugEntity(o.debugEntity)
#endif
  {
  }
  Movement& operator=(Movement&& o) noexcept
  {
    a               = o.a;
    range           = o.range;
    destination     = o.destination;
    reachableTiles  = std::move(o.reachableTiles);
    movementPending.store(o.movementPending.load());
    moving.store(o.moving.load());
#ifdef DEBUG
    debugInfoDisplayed = o.debugInfoDisplayed;
    removeDebugInfo    = o.removeDebugInfo;
    debugEntity        = o.debugEntity;
#endif
    return *this;
  }
};
