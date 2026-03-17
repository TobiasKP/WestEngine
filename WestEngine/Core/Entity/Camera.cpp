#include "../../CoreHeaders/Entity/Camera.h"

#include "glm/gtc/type_ptr.hpp"

#include <Config.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 pos, glm::vec3 rot) : _rotation(rot), _position(pos), _cameraUniforms(-1), _dirty(true) {}

Camera::~Camera() {}

void Camera::update()
{
  assert(_cameraUniforms != -1);
  if (!_dirty.load())
  {
    return;
  }
  _dirty.store(false);
  _projection = getProjectionMatrix();
  _view       = getViewMatrix();
  createFrustumFromCamera();
  glBindBuffer(GL_UNIFORM_BUFFER, _cameraUniforms);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(_view));
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_projection));
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
  x = x * Config::GeneralConfig.SPEED * Config::GeneralConfig.DELTA;
  z = z * Config::GeneralConfig.SPEED * Config::GeneralConfig.DELTA;
  y = y * Config::GeneralConfig.SPEED * Config::GeneralConfig.DELTA;
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
  _dirty.store(true, std::memory_order_relaxed);
}

void Camera::moveRotation(float x, float y, float z)
{
  _rotation.x += x;
  _rotation.y += y;
  _rotation.z += z;
}

void Camera::createFrustumFromCamera()
{
  const float aspect       = (float)Config::GeneralConfig.WIDTH / Config::GeneralConfig.HEIGHT;
  const glm::mat4 inverse  = glm::inverse(_view);
  const glm::mat3 rotation = glm::mat3(inverse);
  const glm::vec3 front = rotation * glm::vec3(0, 0, -1), up = rotation * glm::vec3(0, 1, 0),
                  right = rotation * glm::vec3(1, 0, 0);

  const float halfVSide        = _Z_FAR * tanf(_FOV * 0.5f);
  const float halfHSide        = halfVSide * aspect;
  const glm::vec3 frontMultFar = _Z_FAR * front;

  _FRUSTUM.near   = {_position + _Z_NEAR * front, front};
  _FRUSTUM.far    = {_position + frontMultFar, -front};
  _FRUSTUM.right  = {_position, glm::cross(frontMultFar - right * halfHSide, up)};
  _FRUSTUM.left   = {_position, glm::cross(up, frontMultFar + right * halfHSide)};
  _FRUSTUM.top    = {_position, glm::cross(right, frontMultFar - up * halfVSide)};
  _FRUSTUM.bottom = {_position, glm::cross(frontMultFar + up * halfVSide, right)};
}
