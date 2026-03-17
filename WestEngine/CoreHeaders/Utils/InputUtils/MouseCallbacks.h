#pragma once

#include "../../../Core/Events/EventDispatcher.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class MouseCallbacks
{
public:
  static void mouseCallback(GLFWwindow* window, double x, double y);
  static void enterCallback(GLFWwindow* window, std::int32_t entered);
  static void mouseButtonCallback(GLFWwindow* window, std::int32_t button, std::int32_t action, std::int32_t mods);
  static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
  static void setDispatcher(std::shared_ptr<EventDispatcher> d)
  {
    _dispatcher = d;
  }

private:
  static std::shared_ptr<EventDispatcher> _dispatcher;
  static std::int32_t _inWindow;
};
