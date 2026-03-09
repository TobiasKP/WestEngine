#include "../../CoreHeaders/Systems/PlayerControl.h"

#include "../../Constants/LuaAPI.hpp"
#include "../../Constants/Systems.hpp"
#include "../CoreHeaders/Entity/Scene.h"

#include <Config.h>
#include <format>

PlayerControl::PlayerControl(WestLogger* logger) : ISystem(), _logger(logger), _cameraPending(false)
{
  setName(Systems::PLAYER_CONTROL);
  _movementInitiated.store(false);
  _cleared = false;
#ifdef DEBUG
  _logger->log(Level::Info, std::format("{} *** Initialized debug information\n", getName()));
  _camLog = true;
#endif
};

PlayerControl::~PlayerControl() {}

void PlayerControl::init()
{
  _reg = Scene::getSceneInstance().getRegistry();
  LuaFacade::getLuaFacadeInstance().registerCFunction(movePlayerUnit, LuaAPI::C_MOVE_PLAYER.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(actionFinished, LuaAPI::C_ACTIONF_PLAYER.data(), this);
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

  if (!_movementInitiated.load())
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
  if (!_cleared.exchange(true))
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
  if (!_movementInitiated.load() && inRange)
  {
    movComp->destination = dest;
    movComp->movementPending.store(true);
  }
}

int PlayerControl::movePlayerUnit(lua_State* L)
{
  std::int32_t n = lua_gettop(L);
  assert(n == 1);
  PlayerControl* me = (PlayerControl*)lua_touserdata(L, lua_upvalueindex(1));
  me->_movementInitiated.store(true);
  me->_cleared.store(false);
  return 0;
}

int PlayerControl::actionFinished(lua_State* L)
{
  std::int32_t n = lua_gettop(L);
  assert(n == 1);
  PlayerControl* me = (PlayerControl*)lua_touserdata(L, lua_upvalueindex(1));
  me->_movementInitiated.store(false);
  return 0;
}
