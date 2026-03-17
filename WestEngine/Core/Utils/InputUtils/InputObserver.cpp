#include "../../../CoreHeaders/Utils/InputUtils/InputObserver.h"

#include "../../../Constants/Systems.hpp"
#include "../../../CoreHeaders/SystemManager.h"

using namespace WestInterface;

InputObserver::InputObserver()
{
  _control = (PlayerControl*)SystemManager::getSystemByName(Systems::PLAYER_CONTROL);
  _facade  = &WestInterfaceFacade::getInterfaceInstance();
}

void InputObserver::notify() {}

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
