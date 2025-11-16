#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Camera
{
public:
  Camera() : _rotation(glm::vec3(0)), _position(glm::vec3(0)), _cameraUniforms(-1) {};
  Camera(glm::vec3 pos, glm::vec3 rot) : _rotation(rot), _position(pos), _cameraUniforms(-1) {};
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

  inline glm::vec3 getPosition()
  {
    return _position;
  }

private:
  GLuint _cameraUniforms;
  glm::vec3 _position;
  glm::vec3 _rotation;

  const float _FOV    = glm::radians(60.0f);
  const float _Z_NEAR = 0.01f;
  const float _Z_FAR  = 1000.0f;
};
