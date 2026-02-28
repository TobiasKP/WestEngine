#pragma once

#include <glm/glm.hpp>

struct Plane
{
  glm::vec3 normal = {0.0f, 1.0f, 0.0f};
  float distance   = 0.0f;

  Plane() = default;

  Plane(glm::vec3 point, glm::vec3 norm)
      : normal(glm::normalize(norm)), distance(glm::dot(normal, point))
  {
  }

  float getSignedDistance(const glm::vec3& point) const
  {
    return glm::dot(normal, point) - distance;
  }
};
