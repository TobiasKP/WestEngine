#pragma once

#include "../../InputManager.h"
#include "../../WindowManager.h"

class KeyboardCallbacks
{
public:
  static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
  static void executeBoundOperation(std::int32_t key, const std::string boundOperation);
  static void shutdown();

  // Setter
  static void setInputManager(InputManager* manager)
  {
    _iManager = manager;
  }
  static void setWindowManager(WindowManager* manager)
  {
    _wManager = manager;
  }
  static void setInputObserver(InputObserver* observer)
  {
    _iObserver = observer;
  }

private:
  static InputManager* _iManager;
  static WindowManager* _wManager;
  static InputObserver* _iObserver;
};
