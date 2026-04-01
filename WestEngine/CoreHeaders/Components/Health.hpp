#pragma once

#include <cstdint>
#include <optional>

struct Health
{
  std::int16_t max, current;
  std::optional<std::uint16_t> interface;
};
