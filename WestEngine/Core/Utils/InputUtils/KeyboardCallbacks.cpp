#include "../../../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"

#include <Config.h>
#include <format>

InputManager* KeyboardCallbacks::_iManager   = nullptr;
WindowManager* KeyboardCallbacks::_wManager  = nullptr;
InputObserver* KeyboardCallbacks::_iObserver = nullptr;

void KeyboardCallbacks::shutdown()
{
  assert(_iManager != nullptr && _wManager != nullptr);
  delete _iManager;
  delete _wManager;
}

void KeyboardCallbacks::keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  assert(_iManager != nullptr && _wManager != nullptr && _iObserver != nullptr);

  const std::string command = _iManager->findByKey(key);
  if (command.compare(CoreConstants::UNDEFINED_STRING))
  {
    return;
  }
  else if (command.compare("Pause") == 0 && action == GLFW_PRESS)
  {
    Config::PAUSE = !Config::PAUSE;
  }
  else if (command.compare("OpenMenu") == 0 && action == GLFW_PRESS && !Config::PAUSE)
  {
    _iObserver->setGeneralFlag(BitMasks::General::MENU);
  }
  else
  {
    WestLogger* logger = &WestLogger::getLoggerInstance();
    logger->log(Level::Error,
                std::format("Could not find Operation to execute for key: {} with command: {}\n", key, command));
  }
}

void KeyboardCallbacks::executeBoundOperation(std::int32_t key, const std::string boundOperation)
{
  assert(_iManager != nullptr && _wManager != nullptr && _iObserver != nullptr);
  if (!_wManager->isKeyPressed(key) || Config::PAUSE)
  {
    return;
  }

  std::int32_t x = 0, z = 0;
  bool updateCam = false;
  if (boundOperation.compare("CameraUp") == 0)
  {
    z         = -1;
    updateCam = true;
  }
  else if (boundOperation.compare("CameraDown") == 0)
  {
    z         = 1;
    updateCam = true;
  }
  else if (boundOperation.compare("CameraLeft") == 0)
  {
    x         = -1;
    updateCam = true;
  }
  else if (boundOperation.compare("CameraRight") == 0)
  {
    x         = 1;
    updateCam = true;
  }

  if (updateCam)
  {
    _iObserver->setControlFlag(BitMasks::Control::CAMERA_MOVING, x, z);
  }
}
