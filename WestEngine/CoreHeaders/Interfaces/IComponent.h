#pragma once

#include <glm/glm.hpp>
#include <cassert>

#include "../../Constants/BitMasks.h"

struct IComponent {
 const std::uint16_t _guid = 0;

  // Overload
  bool operator<(const IComponent &other) const { return _guid < other._guid; } 
};
