#include "../../../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include "../../../CoreHeaders/Entity/Scene.h"
#include "../../../CoreHeaders/Utils/Math/PositionCalculation.h"

glm::vec2 MouseCallbacks::_currentPos = glm::vec2(0.0f);

std::int32_t MouseCallbacks::_inWindow = 0;
std::int32_t MouseCallbacks::_leftButtonPress = 0;
std::int32_t MouseCallbacks::_rightButtonPress = 0;
std::int16_t MouseCallbacks::_currentHover = -1;
std::vector<ElementBounds *> MouseCallbacks::_elements;

InputObserver *MouseCallbacks::_iObserver = nullptr;

void MouseCallbacks::mouseCallback(GLFWwindow *window, double x, double y) {
  _currentPos.x = x;
  _currentPos.y = y;
  if (!_inWindow)
    return;

  std::int16_t hover = isInterfaceHovered();

  if (hover != _currentHover) {
    if (_currentHover != -1) {
      _iObserver->setControlFlag(BitMasks::Control::UI_UNHOVERED, _currentHover);
    }

    if (hover != -1) {
      _iObserver->setControlFlag(BitMasks::Control::UI_HOVERED, hover);
    }
  }

  _currentHover = hover;
}

void MouseCallbacks::enterCallback(GLFWwindow *window, std::int32_t entered) {
  _inWindow = entered;
}

void MouseCallbacks::mouseButtonCallback(GLFWwindow *window,
                                         std::int32_t button,
                                         std::int32_t action,
                                         std::int32_t mods) {
  assert(_iObserver != nullptr);
  if (!_inWindow)
    return;

  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
    glm::vec3 destination = PositionCalculation::getWorldPosition(
        _currentPos, Scene::getSceneInstance().getCamera());
    _iObserver->setControlFlag(BitMasks::Control::PLAYER_MOVING, destination);
  }
  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
    // TODO More actions
  };
}

std::int16_t MouseCallbacks::isInterfaceHovered() {
  for (ElementBounds *eb : _elements) {
    if (_currentPos.x > eb->xLeft && _currentPos.x < eb->xRight &&
        _currentPos.y < Config::GeneralConfig.HEIGHT - eb->yBottom &&
        _currentPos.y > Config::GeneralConfig.HEIGHT - eb->yTop)
      return eb->id;
  }
  return -1;
}
