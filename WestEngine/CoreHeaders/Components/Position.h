#pragma once

#include "../Interfaces/IComponent.h"

#include <GL/glew.h>

struct Position : public IComponent
{
  glm::vec3 position = glm::vec3(1.0f), rotation = glm::vec3(1.0f);
  std::int32_t tile = -1;
  float scale       = 1.0f;
  GLuint uniform    = -1;
};
