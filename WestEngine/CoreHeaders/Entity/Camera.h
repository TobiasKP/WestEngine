#pragma once

#include "../../CoreHeaders/Utils/Math/Plane.hpp"

#include <atomic>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <ThreadSafeQueue.hpp>

struct Frustum
{
  Plane near, far, left, right, top, bottom;
};

class Camera
{
public:
  Camera() : _rotation(glm::vec3(0)), _position(glm::vec3(0)), _cameraUniforms(-1), _dirty(true) {};
  Camera(glm::vec3 pos, glm::vec3 rot);
  ~Camera();

  void update();
  void movePosition(float x, float y, float z);
  void moveRotation(float x, float y, float z);
  glm::mat4 getViewMatrix();
  glm::mat4 getProjectionMatrix();

  inline void setPosition(glm::vec3 pos)
  {
    _position = pos;
  }
  inline void setRotation(glm::vec3 rot)
  {
    _rotation = rot;
  }
  inline void setCameraUniforms(GLuint u)
  {
    _cameraUniforms = u;
  }
  inline glm::vec3& getPosition()
  {
    return _position;
  }
  inline Frustum& getFrustum()
  {
    return _FRUSTUM;
  }

private:
  void createFrustumFromCamera();


  GLuint _cameraUniforms;
  glm::vec3 _move;
  glm::vec3 _position;
  glm::vec3 _rotation;
  glm::mat4 _view;
  glm::mat4 _projection;
  std::atomic<bool> _dirty;

  const float _FOV    = glm::radians(60.0f);
  const float _Z_NEAR = 0.01f;
  const float _Z_FAR  = 1000.0f;
  Frustum _FRUSTUM;
};
