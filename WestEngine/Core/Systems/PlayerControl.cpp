#include "../../CoreHeaders/Systems/PlayerControl.h"

#include "../../Constants/Systems.hpp"
#include "../CoreHeaders/Entity/Scene.h"

#include <Config.h>
#include <format>

PlayerControl::PlayerControl(WestLogger* logger) : ISystem(), _cameraPending(false)
{
  setName(Systems::PLAYER_CONTROL);
  _logger = logger;
#ifdef DEBUG
  _logger->log(Level::Info, std::format("{} *** Initialized debug information\n", getName()));
  _camLog = true;
#endif
};

PlayerControl::~PlayerControl() {}

void PlayerControl::init()
{
  _reg = Scene::getSceneInstance().getRegistry();
}

void PlayerControl::update()
{
  std::shared_ptr<ComponentArray<Control>> controlUnits = _reg->getComponentArray<Control>();
  std::uint32_t id                                      = controlUnits->getComponents()[0].entityId;
  if (_cameraPending.exchange(false))
  {
    glm::vec3 localCam;
    {
      std::lock_guard<std::mutex> lock(_CameraMutex);
      localCam = _moveCamera;
    }

#ifdef DEBUG
    if (_camLog)
    {
      _logger->log(
        Level::Info,
        std::format("{} *** updating Camera position ({}, {}, {})\n", getName(), localCam.x, localCam.y, localCam.z));
      _camLog = false;
    }
#endif
    updateCamera(localCam);
  }
#ifdef DEBUG
  else
  {
    _camLog = true;
  }
#endif
  if (_state == LuaFacade::LuaStates::IDLE)
  {
    Entity* e         = Scene::getSceneInstance().getEntityById(id);
    Movement* movComp = _reg->getComponent<Movement>(e->getId());
    assert(movComp != nullptr);
    Position* posComp = _reg->getComponent<Position>(e->getId());
    assert(posComp != nullptr);
    World* w                      = Scene::getSceneInstance().getWorld();
    std::int32_t tileIdx          = w->calculateIndex(posComp->position.x, posComp->position.z);
    std::int32_t dimension        = w->getGridSize();
    std::int32_t column           = tileIdx % dimension;
    std::int32_t row              = tileIdx / dimension;
    std::vector<std::int32_t> res = w->getReachableTiles(row, column, movComp->range, movComp->a, this);
    movComp->reachableTiles       = res;
  }
  if (_state == LuaFacade::LuaStates::MOVING)
  {
    World* w = Scene::getSceneInstance().getWorld();
    w->clearFlag(0x0002u);
  }
}

void PlayerControl::updateCamera(glm::vec3 local)
{
  Camera* camera = Scene::getSceneInstance().getCamera();
  assert(camera != nullptr);
  camera->movePosition(local.x, local.y, local.z);
  _moveCamera = glm::vec3(0.0f);
}

void PlayerControl::setCameraMovement(glm::vec3 move)
{
  std::unique_lock<std::mutex> lock(_CameraMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    _moveCamera += move;
    _cameraPending.store(true);
  }
}

void PlayerControl::updateDebuggingInfo() {}

void PlayerControl::passDestinationPosition(glm::vec3 dest)
{
  std::shared_ptr<ComponentArray<Control>> controlUnits = _reg->getComponentArray<Control>();
  std::uint32_t id                                      = controlUnits->getComponents()[0].entityId;
  Entity* e                                             = Scene::getSceneInstance().getEntityById(id);
  Movement* movComp                                     = _reg->getComponent<Movement>(e->getId());
  assert(movComp != nullptr);
  std::int32_t tile = Scene::getSceneInstance().getWorld()->calculateIndex(dest.x, dest.z);
  bool inRange =
    std::find(movComp->reachableTiles.begin(), movComp->reachableTiles.end(), tile) != movComp->reachableTiles.end();
  if (_state == LuaFacade::LuaStates::IDLE && inRange)
  {
    movComp->destination = dest;
    bool result = LuaFacade::getLuaFacadeInstance().onTileClicked(id, LuaFacade::MouseAction::LMOUSE_CLICK, dest);
    if (result)
    {
      _logger->log(Level::Info,
                   std::format("{} *** Error calling lua function: {}\n",
                               getName(),
                               (std::int32_t)LuaFacade::MouseAction::LMOUSE_CLICK));
    }
  }
}
