#pragma once

#include "../Interfaces/IComponent.h"

#include <glm/glm.hpp>

struct AABB : public IComponent
{
  glm::vec3 min, max;
};
