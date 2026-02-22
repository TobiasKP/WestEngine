#pragma once

#include "../../Constants/BitMasks.hpp"

#include <atomic>
#include <cassert>
#include <glm/glm.hpp>

struct IComponent
{
  const std::uint16_t _guid = 0;
  std::atomic<bool> _dirty  = true;

  // Overload
  bool operator<(const IComponent& other) const
  {
    return _guid < other._guid;
  }
};
