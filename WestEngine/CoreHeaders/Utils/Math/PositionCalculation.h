#pragma once

#include "../../Entity/Camera.h"

class PositionCalculation
{
public:
  static glm::vec3 getWorldPosition(glm::vec2 screenPosition, Camera* camera);
  static glm::mat4 createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, float scale);
};
