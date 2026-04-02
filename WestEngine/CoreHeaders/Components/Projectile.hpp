#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Projectile
{
  std::uint32_t speed;
  std::uint32_t damage;
  std::uint32_t destination;
  bool hit;
};
