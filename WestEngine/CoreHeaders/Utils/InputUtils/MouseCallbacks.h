#pragma once

#include "InputObserver.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class MouseCallbacks
{
public:
  static void mouseCallback(GLFWwindow* window, double x, double y);
  static void enterCallback(GLFWwindow* window, std::int32_t entered);
  static void mouseButtonCallback(GLFWwindow* window, std::int32_t button, std::int32_t action, std::int32_t mods);

  inline static void setInputObserver(InputObserver* o)
  {
    _iObserver = o;
  }
  inline static void setElementBounds(std::vector<ElementBounds*>& elements)
  {
    _elements = elements;
  };

private:
  static std::tuple<std::int16_t, bool> isInterfaceHovered();

  static std::vector<ElementBounds*> _elements;

  static glm::vec2 _currentPos;
  static std::tuple<std::int16_t, bool> _currentHover;
  static std::int32_t _inWindow, _tileIdx;
  static InputObserver* _iObserver;
};
