#pragma once

#include <cstdint>

struct Weapon
{
  std::uint32_t id;
  std::uint32_t dmg;
  std::uint32_t range;
  float accuracy;
};

struct Equipment
{
  Weapon primary;
  Weapon secondary;
  std::uint8_t active = 2;
};
