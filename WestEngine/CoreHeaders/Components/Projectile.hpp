#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Projectile
{
  float speed;
  std::uint32_t damage;
  std::uint32_t destination;
  std::uint32_t owner;
  bool hit;
};
