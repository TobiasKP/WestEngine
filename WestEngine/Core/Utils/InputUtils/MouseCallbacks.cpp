#include "../../../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include "../../../Constants/InternalEvents.hpp"

std::shared_ptr<EventDispatcher> MouseCallbacks::_dispatcher;
std::int32_t MouseCallbacks::_inWindow;

void MouseCallbacks::mouseCallback(GLFWwindow* window, double x, double y)
{
  if (!_inWindow)
  {
    return;
  }
  assert(_dispatcher != nullptr);
  _dispatcher->dispatchEvent(EventIdentifiers::MOUSE_MOVE, MousePayload(x, y));

  /*
  if (!_inWindow)
  {
    return;
  }

  glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(_currentPos, Scene::getSceneInstance().getCamera());
  _tileIdx                = Scene::getSceneInstance().getWorld()->worldPosToTile(hoverPosition.x, hoverPosition.z);
  _iObserver->setTileIdx(_tileIdx);
  _currentHover = hover;*/
}

void MouseCallbacks::enterCallback(GLFWwindow* window, std::int32_t entered)
{
  _inWindow = entered;
}

void MouseCallbacks::mouseButtonCallback(GLFWwindow* window,
                                         std::int32_t button,
                                         std::int32_t action,
                                         std::int32_t mods)
{
  if (!_inWindow)
  {
    return;
  }
  assert(_dispatcher != nullptr);
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {
    _dispatcher->dispatchEvent(EventIdentifiers::MOUSE_LCLICK, EmptyPayload{});
  }

  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
  {
    _dispatcher->dispatchEvent(EventIdentifiers::MOUSE_RCLICK, EmptyPayload{});
  }

  /*
  assert(_iObserver != nullptr);
  if (!_inWindow)
  {
    return;
  }

  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {
    std::tuple<std::int16_t, bool> hover = isInterfaceHovered();
    if (std::get<0>(hover) != -1 && std::get<1>(hover))
    {
      _iObserver->setControlFlag(BitMasks::Control::UI_CLICKED, std::get<0>(hover));
    }
    else if (std::get<0>(hover) == -1)
    {
      _iObserver->setControlFlag(BitMasks::Control::LCLICK);
    }
  }
  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
  {
    _iObserver->setControlFlag(BitMasks::Control::RCLICK);
  };*/
}

void MouseCallbacks::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
  _dispatcher->dispatchEvent(EventIdentifiers::MOUSE_WHEEL, MousePayload{0, 0, yOffset * -5});
}
