#include "../../../CoreHeaders/Utils/InputUtils/MouseCallbacks.h"

#include "../../../CoreHeaders/Entity/Scene.h"
#include "../../../CoreHeaders/Utils/Math/PositionCalculation.h"

glm::vec2 MouseCallbacks::_currentPos = glm::vec2(0.0f);

std::int32_t MouseCallbacks::_inWindow                       = 0;
std::int32_t MouseCallbacks::_tileIdx                        = -1;
std::tuple<std::int16_t, bool> MouseCallbacks::_currentHover = std::make_tuple(-1, false);
std::vector<ElementBounds*> MouseCallbacks::_elements;

InputObserver* MouseCallbacks::_iObserver = nullptr;

void MouseCallbacks::mouseCallback(GLFWwindow* window, double x, double y)
{
  _currentPos.x = x;
  _currentPos.y = y;
  if (!_inWindow)
  {
    return;
  }

  std::tuple<std::int16_t, bool> hover = isInterfaceHovered();
  if (std::get<0>(hover) != std::get<0>(_currentHover))
  {
    if (std::get<0>(_currentHover) != -1 && std::get<1>(_currentHover))
    {
      _iObserver->setControlFlag(BitMasks::Control::UI_UNHOVERED, std::get<0>(_currentHover));
    }

    if (std::get<0>(hover) != -1 && std::get<1>(hover))
    {
      _iObserver->setControlFlag(BitMasks::Control::UI_HOVERED, std::get<0>(hover));
    }
  }
  glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(_currentPos, Scene::getSceneInstance().getCamera());
  _tileIdx                = Scene::getSceneInstance().getWorld()->worldPosToTile(hoverPosition.x, hoverPosition.z);

  _currentHover = hover;
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
      std::optional<glm::vec3> res = Scene::getSceneInstance().getWorld()->tileToWorldPos(_tileIdx);
      if (!res.has_value())
      {
        return;
      }
      _iObserver->setControlFlag(BitMasks::Control::PLAYER_MOVING, res.value());
    }
  }
  if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
  {
    // TODO More actions
  };
}

std::tuple<std::int16_t, bool> MouseCallbacks::isInterfaceHovered()
{
  for (ElementBounds* eb : _elements)
  {
    if (_currentPos.x > eb->xLeft && _currentPos.x < eb->xRight && _currentPos.y < eb->yBottom
        && _currentPos.y > eb->yTop)
    {
      return std::make_tuple(eb->id, eb->eventDriven);
    }
  }
  return std::make_tuple(-1, false);
}
