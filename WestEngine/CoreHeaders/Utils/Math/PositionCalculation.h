#pragma once

#include "../../Entity/Camera.h"

#include <memory>

class PositionCalculation
{
public:
  static glm::vec3 getWorldPosition(glm::vec2 screenPosition, const std::shared_ptr<Camera>& camera);
  static glm::vec2 getScreenPosition(glm::vec3 worldPos, const std::shared_ptr<Camera>& camera);
  static glm::mat4 createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, float scale);
};
