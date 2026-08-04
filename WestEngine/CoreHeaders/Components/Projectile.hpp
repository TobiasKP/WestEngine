#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Projectile
{
  float speed;
  std::uint32_t damage;
  std::uint32_t destination;
  bool hit;
};
