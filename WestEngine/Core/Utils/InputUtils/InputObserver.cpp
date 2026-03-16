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

    entityHovering();


    _interfaceHoverId = _interfaceUnhoverId = -1;
    _controlFlags                           = {0b0000'0000};
  }
}

void InputObserver::entityHovering()
{
  // TODO duplicated code here and in LCLICK
  World* w         = Scene::getSceneInstance().getWorld();
  std::uint32_t id = w->getEntityByIdx(_tileIdx);
  Material* m;
  if (_lastEntityHover != -1)
  {
    m                = Scene::getSceneInstance().getRegistry()->getComponent<Material>(_lastEntityHover);
    m->emissiveColor = glm::vec3(0.0, 0.0, 0.0);
  }
  if (id > 0)
  {
    m                = Scene::getSceneInstance().getRegistry()->getComponent<Material>(id);
    m->emissiveColor = glm::vec3(0.0, 0.5, 0.5);
    _lastEntityHover = id;
  }
}
