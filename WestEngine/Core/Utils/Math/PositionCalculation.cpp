#include "../../../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <Config.h>
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 PositionCalculation::getWorldPosition(glm::vec2 screenPosition, Camera* camera)
{
  float ndcX = (2.0f * screenPosition.x) / Config::GeneralConfig.WIDTH - 1.0f;
  float ndcY = 1.0f - (2.0f * screenPosition.y) / Config::GeneralConfig.HEIGHT;
  glm::vec4 screenPos(ndcX, ndcY, -1.0f, 1.0f);

  glm::mat4 inverseVP  = glm::inverse(camera->getProjectionMatrix() * camera->getViewMatrix());
  glm::vec4 worldPos   = inverseVP * screenPos;
  worldPos            /= worldPos.w;

  float planeY          = 0.0f;
  float t               = (planeY - camera->getPosition().y) / (worldPos.y - camera->getPosition().y);
  glm::vec3 destination = glm::vec3(camera->getPosition().x + t * (worldPos.x - camera->getPosition().x),
                                    planeY,
                                    camera->getPosition().z + t * (worldPos.z - camera->getPosition().z));

  return destination;
}

glm::mat4 PositionCalculation::createTransformationMatrix(glm::vec3 position, glm::vec3 rotation, float scale)
{
  glm::mat4 mat = glm::mat4(1.0f);
  mat           = glm::translate(mat, position);
  mat           = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  mat           = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  mat           = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  mat           = glm::scale(mat, glm::vec3(scale, scale, scale));
  return mat;
}
