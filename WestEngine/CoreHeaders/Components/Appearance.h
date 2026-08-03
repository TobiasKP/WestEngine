#pragma once

#include <glm/glm.hpp>

struct Appearance
{
  glm::vec3 diffuseOverride  = glm::vec3(0.0f);
  glm::vec3 emissiveOverride = glm::vec3(0.0f);
};
