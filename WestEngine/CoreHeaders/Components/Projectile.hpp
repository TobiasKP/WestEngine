#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct Projectile
{
  std::int32_t speed;
  std::int32_t damage;
  glm::vec3 destionation;
};
