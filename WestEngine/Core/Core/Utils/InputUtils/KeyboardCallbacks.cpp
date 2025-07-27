#include "../../../CoreHeaders/Utils/InputUtils/KeyboardCallbacks.h"

#include "../../../Config/Config.h"

InputManager *KeyboardCallbacks::_iManager = nullptr;
WindowManager *KeyboardCallbacks::_wManager = nullptr;
InputObserver *KeyboardCallbacks::_iObserver = nullptr;

void KeyboardCallbacks::shutdown() {
  assert(_iManager != nullptr && _wManager != nullptr);
  delete _iManager;
  delete _wManager;
}

void KeyboardCallbacks::keyboardCallback(GLFWwindow *window, int key,
                                         int scancode, int action, int mods) {

  assert(_iManager != nullptr && _wManager != nullptr && _iObserver != nullptr);

  const char *command = _iManager->findByKey(key);
  if (command == nullptr) {
    return;
  } else if (strcmp(command, "Pause") == 0 && action == GLFW_PRESS) {
    Config::EngineInternals.PAUSE = !Config::EngineInternals.PAUSE;
  } else if (strcmp(command, "OpenMenu") == 0 && action == GLFW_PRESS &&
             !Config::EngineInternals.PAUSE) {
    _iObserver->setGeneralFlag(BitMasks::General::MENU);
  }
}

void KeyboardCallbacks::executeBoundOperation(std::int32_t key,
                                              const char *boundOperation) {
  assert(_iManager != nullptr && _wManager != nullptr && _iObserver != nullptr);
  if (!_wManager->isKeyPressed(key) || Config::EngineInternals.PAUSE) {
    return;
  }

  std::int32_t x = 0, y = 0;
  bool updateCam = false;
  if (strcmp(boundOperation, "CameraUp") == 0) {
    y = 1;
    updateCam = true;
  } else if (strcmp(boundOperation, "CameraDown") == 0) {
    y = -1;
    updateCam = true;
  } else if (strcmp(boundOperation, "CameraLeft") == 0) {
    x = -1;
    updateCam = true;
  } else if (strcmp(boundOperation, "CameraRight") == 0) {
    x = 1;
    updateCam = true;
  }

  if (updateCam) {
    _iObserver->setControlFlag(BitMasks::Control::CAMERA_MOVING, x, y);
  }
}
