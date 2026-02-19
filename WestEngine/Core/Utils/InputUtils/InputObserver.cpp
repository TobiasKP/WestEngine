#include "../../../CoreHeaders/Utils/InputUtils/InputObserver.h"

#include "../../../Constants/Systems.hpp"
#include "../../../CoreHeaders/SystemManager.h"

using namespace WestInterface;

InputObserver::InputObserver()
{
  _control = (PlayerControl*)SystemManager::getSystemByName(Systems::PLAYER_CONTROL);
  _facade  = &WestInterfaceFacade::getInterfaceInstance();
}

void InputObserver::notify()
{
  assert(_control != nullptr);
  {
    std::lock_guard<std::mutex> lock(_generalMutex);
    if (static_cast<bool>(_generalFlags & BitMasks::General::MENU))
    {}
    // TODO set in Interface Manager

    _generalFlags = {0b0000'0000};
  }
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    if (static_cast<bool>(_controlFlags & BitMasks::Control::UI_CLICKED))
    {
      assert(_interfaceHoverId != -1);
      _facade->notify(_interfaceHoverId, 0x04, -1, -1);
    }

    if (static_cast<bool>(_controlFlags & BitMasks::Control::UI_HOVERED))
    {
      assert(_interfaceHoverId != -1);
      _facade->notify(_interfaceHoverId, 0x01, _x, _z);
    }

    if (static_cast<bool>(_controlFlags & BitMasks::Control::UI_UNHOVERED))
    {
      assert(_interfaceUnhoverId != -1);
      _facade->notify(_interfaceUnhoverId, 0x02, -1, -1);
    }

    if (static_cast<bool>(_controlFlags & BitMasks::Control::CAMERA_MOVING))
    {
      _control->setCameraMovement(glm::vec3(_x, 0, _z));
      _x = 0;
      _z = 0;
    }

    if (static_cast<bool>(_controlFlags & BitMasks::Control::PLAYER_MOVING))
    {
      _control->passDestinationPosition(_playerDestination);
    }

    _interfaceHoverId = _interfaceUnhoverId = -1;
    _controlFlags                           = {0b0000'0000};
  }
}
