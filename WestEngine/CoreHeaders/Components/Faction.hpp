#pragma once

#include "ComponentRegistry.hpp"

inline bool isHostile(ComponentRegistry& reg, std::uint32_t a, std::uint32_t b)
{
  Control* ca = reg.getComponent<Control>(a);
  Control* cb = reg.getComponent<Control>(b);
  return ca != nullptr && cb != nullptr && ca->aiControl != cb->aiControl;
}
