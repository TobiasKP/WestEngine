#pragma once

#include <../../../Libs/GLM/glm.hpp>
#include <cassert>

#include "../../Constants/BitMasks.h"

struct IComponent {
 const std::uint16_t _guid = NULL;

  // Overload
  bool operator<(const IComponent &other) const { return _guid < other._guid; } 
};
