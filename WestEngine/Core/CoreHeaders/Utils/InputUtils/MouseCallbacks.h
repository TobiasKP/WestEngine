#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "InputObserver.h"

class MouseCallbacks {

public:
  static void mouseCallback(GLFWwindow *window, double x, double y);
  static void enterCallback(GLFWwindow *window, std::int32_t entered);
  static void mouseButtonCallback(GLFWwindow *window, std::int32_t button,
                                  std::int32_t action, std::int32_t mods);

  inline static void setInputObserver(InputObserver *o) { _iObserver = o; }

private:
  static glm::vec2 _currentPos;
  static std::int32_t _inWindow, _leftButtonPress, _rightButtonPress;
  static InputObserver *_iObserver;
};
