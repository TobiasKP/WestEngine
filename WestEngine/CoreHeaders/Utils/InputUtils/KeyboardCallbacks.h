#pragma once

#include "../../../Core/Events/EventDispatcher.hpp"

#include <GLFW/glfw3.h>
#include <memory>

class KeyboardCallbacks
{
public:
  static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void setDispatcher(std::shared_ptr<EventDispatcher> d)
  {
    _dispatcher = d;
  }

private:
  static std::shared_ptr<EventDispatcher> _dispatcher;
};
