#include "../../../CoreHeaders/Utils/Math/PositionCalculation.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <Config.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>


glm::vec3 PositionCalculation::getWorldPosition(glm::vec2 screenPosition, const std::shared_ptr<Camera>& camera)
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

glm::vec2 PositionCalculation::getScreenPosition(glm::vec3 worldPos, const std::shared_ptr<Camera>& camera)
{
  glm::vec4 clip = camera->getProjectionMatrix() * camera->getViewMatrix() * glm::vec4(worldPos, 1.0f);
  glm::vec3 ndc  = glm::vec3(clip) / clip.w;
  float screenX  = (ndc.x * 0.5f + 0.5f) * Config::GeneralConfig.WIDTH;
  float screenY  = (ndc.y * 0.5f + 0.5f) * Config::GeneralConfig.HEIGHT;
  return glm::vec2(screenX, screenY);
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

void PositionCalculation::updatePosition(glm::vec3 direction, Position* posComp, float travelSpeed)
{
  glm::vec3 update = direction - posComp->position;

  if (glm::length2(update) <= 0.025f)
  {
    posComp->position = direction;
  }
  else
  {
    direction = (glm::normalize(update) * Config::GeneralConfig.SPEED * Config::GeneralConfig.DELTA) * travelSpeed;
    posComp->position += direction;
  }
}
