#include "../../CoreHeaders/Entity/Camera.h"

#include "glm/gtc/type_ptr.hpp"

#include <Config.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::~Camera() {}

void Camera::update()
{
  assert(_cameraUniforms != -1);
  glBindBuffer(GL_UNIFORM_BUFFER, _cameraUniforms);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(getViewMatrix()));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(getProjectionMatrix()));
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4 Camera::getProjectionMatrix()
{
  float aspectRatio = (float)Config::GeneralConfig.WIDTH / Config::GeneralConfig.HEIGHT;
  glm::mat4 mat     = glm::perspective(_FOV, aspectRatio, _Z_NEAR, _Z_FAR);
  return mat;
}

glm::mat4 Camera::getViewMatrix()
{
  glm::mat4 mat = glm::mat4(1.0f);
  mat           = glm::rotate(mat, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
  mat           = glm::rotate(mat, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
  mat           = glm::rotate(mat, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
  mat           = glm::translate(mat, -_position);
  return mat;
}

void Camera::movePosition(float x, float y, float z)
{
  x = x * Config::GeneralConfig.SPEED;
  z = z * Config::GeneralConfig.SPEED;
  y = y * Config::GeneralConfig.SPEED;
  if (z != 0)
  {
    _position.x += glm::sin(glm::radians(_rotation.y)) * -1.0f * z;
    _position.z += glm::cos(glm::radians(_rotation.y)) * z;
  }
  if (x != 0)
  {
    _position.x += glm::sin(glm::radians(_rotation.y - 90)) * -1.0f * x;
    _position.z += glm::cos(glm::radians(_rotation.y - 90)) * x;
  }
  _position.y += y;
}

void Camera::moveRotation(float x, float y, float z)
{
  _rotation.x += x;
  _rotation.y += y;
  _rotation.z += z;
}
