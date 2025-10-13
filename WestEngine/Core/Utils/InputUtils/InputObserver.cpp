#include "../../../CoreHeaders/Utils/InputUtils/InputObserver.h"

#include "../../../Constants/Systems.h"
#include "../../../CoreHeaders/SystemManager.h"


InputObserver::InputObserver() {
  _control =
      (PlayerControl *)SystemManager::getSystemByName(Systems::PLAYER_CONTROL);
}

void InputObserver::notify() {
  assert(_control != nullptr);
  {
    std::lock_guard<std::mutex> lock(_generalMutex);
    if (static_cast<bool>(_generalFlags & BitMasks::General::MENU)) {
    }
    // TODO set in Interface Manager

    _generalFlags = {0b0000'0000};
  }
  {
    std::lock_guard<std::mutex> lock(_controlMutex);
    if (static_cast<bool>(_controlFlags & BitMasks::Control::CAMERA_MOVING)) {
      _control->setCameraMovement(
          glm::vec3(_x, _y, 0));
      _x = 0;
      _y = 0;
    }

    if (static_cast<bool>(_controlFlags & BitMasks::Control::PLAYER_MOVING))
      _control->setDestinationPosition(_playerDestination);

    _controlFlags = {0b0000'0000};
  }
}
