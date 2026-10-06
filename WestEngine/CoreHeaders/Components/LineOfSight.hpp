#pragma once

#include "Movement.hpp"

#include <cstdint>
#include <optional>

struct LineOfSight
{
  std::int32_t range = 0;
};

inline std::optional<std::int32_t> getLineOfSightRange(const LineOfSight* los, const Movement* mov)
{
  if (los)
  {
    return los->range;
  }
  if (mov)
  {
    return mov->range;
  }
  return {};
}
