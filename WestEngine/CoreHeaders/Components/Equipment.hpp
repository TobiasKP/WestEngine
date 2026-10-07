#pragma once

#include <cstdint>

struct Weapon
{
  std::uint32_t id;
  std::uint32_t dmg;
  std::uint32_t range;
  std::uint32_t bulletType;
  float accuracy;
};

struct Equipment
{
  Weapon primary;
  Weapon secondary;
  std::uint8_t active = 2;

  Weapon* activeWeapon()
  {
    if (active == 1)
    {
      return &primary;
    }
    if (active == 2)
    {
      return &secondary;
    }
    return nullptr;
  }
};
