#pragma once

#include <glm/glm.hpp>

struct Material
{
  glm::vec3 baseColor     = glm::vec3(1.0, 0.0, 0.0);
  glm::vec3 emissiveColor = glm::vec3(0);
  glm::vec3 diffuseColor  = glm::vec3(0);
};
