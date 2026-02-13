#include "../../CoreHeaders/Systems/PlayerControl.h"

#include "../../Constants/LuaAPI.hpp"
#include "../../Constants/Systems.hpp"
#include "../CoreHeaders/Entity/Scene.h"

#include <Config.h>
#include <format>

PlayerControl::PlayerControl(WestLogger* logger) : ISystem(), _logger(logger), _cameraPending(false)
{
  setName(Systems::PLAYER_CONTROL);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("{} *** Initialized debug information", getName()));
  _debugDrawUtils = new DebugDrawUtils(_logger);
  _debugEntityId  = 0;
  _drawn = false, _camLog = true, _posLog = true;
#endif
};

PlayerControl::~PlayerControl() {}

void PlayerControl::init()
{
  LuaFacade::getLuaFacadeInstance().registerCFunction(movePlayerUnit, LuaAPI::C_MOVE_PLAYER, this);
}

void PlayerControl::update()
{
  std::vector<uint32_t> ids = getEntitieIds();
  assert(_logger != nullptr && !ids.empty());
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

  bool pending      = _movementPending.exchange(false);
  Position* posComp = nullptr;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    if (ids.empty())
    {
      _logger->log(Level::Info, std::format("{} *** No entitie for updating Entite in Player Control\n", getName()));
      return;
    }
    std::uint32_t id = ids.front();
    Entity* e        = Scene::getSceneInstance().getEntityById(id);
    posComp          = (Position*)e->getComponent(BitMasks::Components::POSITION);
    assert(posComp != nullptr);
  }
  if (pending)
  {
    LuaFacade::getLuaFacadeInstance().onTileClicked(
      ids.front(), LuaFacade::MouseAction::LMOUSE_CLICK, _moveToDestination);
  }
  if (!destinationReached(posComp))
  {
    glm::vec3 localDest;
#ifdef DEBUG
    if (_debugEntityId != 0 && pending)
    {
      Entity* debugEntity = Scene::getSceneInstance().getEntityById(_debugEntityId);
      if (debugEntity != nullptr && !debugEntity->isDestroyed())
      {
        _logger->log(Level::Info, std::format("{} *** Destroying destination Debug Line\n", getName()));
        debugEntity->destroy();
        _drawn = false;
      }
    }
#endif
    {
      std::lock_guard<std::mutex> lock(_MovementMutex);
      localDest = _moveToDestination;
    }
#ifdef DEBUG
    if (_posLog)
    {
      glm::vec3 currentPos = posComp->position;
      _logger->log(Level::Info,
                   std::format("{} *** Moving entity at position: ({}, {}, {}) - to "
                               "position: ({}, {}, {})\n",
                               getName(),
                               currentPos.x,
                               currentPos.y,
                               currentPos.z,
                               _moveToDestination.x,
                               _moveToDestination.y,
                               _moveToDestination.z));
      _posLog = false;
    }
#endif
    updatePosition(localDest, posComp);
  }

#ifdef DEBUG
  else
  {
    _posLog = true;
  }
#endif
}

void PlayerControl::updateCamera(glm::vec3 local)
{
  Camera* camera = Scene::getSceneInstance().getCamera();
  assert(camera != nullptr);
  camera->movePosition(local.x, local.y, local.z);
}

void PlayerControl::updatePosition(glm::vec3 local, Position* posComp)
{
  assert(posComp != nullptr);
  glm::vec3 direction = local - posComp->position;
  if (glm::length2(direction) <= Config::GeneralConfig.SPEED * Config::GeneralConfig.SPEED)
  {
    posComp->position = local;
    return;
  }
  direction          = glm::normalize(direction) * Config::GeneralConfig.SPEED;
  posComp->position += direction;
}

bool PlayerControl::destinationReached(Position* posComp)
{
  bool reached = glm::all(glm::epsilonEqual(posComp->position, _moveToDestination, Config::GeneralConfig.EPSILON));
#ifdef DEBUG
  if (_debugEntityId != 0 && reached)
  {
    Entity* debugEntity = Scene::getSceneInstance().getEntityById(_debugEntityId);
    if (debugEntity != nullptr && !debugEntity->isDestroyed())
    {
      _logger->log(Level::Info, std::format("{} *** Destroying destination Debug Line\n", getName()));
      debugEntity->destroy();
      _drawn = false;
    }
  }
#endif
  return reached;
}

void PlayerControl::setCameraMovement(glm::vec3 move)
{
  std::unique_lock<std::mutex> lock(_CameraMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    _moveCamera = move;
    _cameraPending.store(true);
  }
}

void PlayerControl::setDestinationPosition(glm::vec3 dest)
{
  std::unique_lock<std::mutex> lock(_MovementMutex, std::try_to_lock);
  if (lock.owns_lock())
  {
    _moveToDestination = dest;
    _movementPending.store(true);
  }
}

void PlayerControl::updateDebuggingInfo()
{
  if (_debugEntityId != 0)
  {
#ifdef DEBUG
    _logger->log(Level::Cycle, std::format("{} *** Retreiving debug entity.\n", getName()));
#endif
    Entity* debugEntity = Scene::getSceneInstance().getEntityById(_debugEntityId);
    if (debugEntity != nullptr && debugEntity->isDestroyed())
    {
      _debugDrawUtils->unloadModel(*debugEntity);
    }
  }

  if (_drawn)
  {
    return;
  }

  std::vector<std::uint32_t> ids = getEntitieIds();
  if (ids.empty())
  {
    _logger->log(Level::Info, std::format("{} *** No entitie for updating Debugging Info\n", getName()));
    return;
  }

  std::uint32_t id = ids.front();
  Entity* e        = Scene::getSceneInstance().getEntityById(id);
  assert(e != nullptr);
  Position* posComp = (Position*)e->getComponent(BitMasks::Components::POSITION);
  assert(posComp != nullptr);
#ifdef DEBUG
  if (posComp == nullptr)
  {
    _logger->log(
      Level::Error,
      std::format("{} *** updateDebuggingInfo: Entity {} has no Position component!\n", getName(), e->getId()));
    return;
  }
#endif
  if (posComp == nullptr)
  {
    return;
  }
  glm::vec3 direction = _moveToDestination - posComp->position;
  if (glm::length2(direction) <= Config::GeneralConfig.SPEED * Config::GeneralConfig.SPEED)
  {
    return;
  }

  _logger->log(Level::Info,
               std::format("{} *** Drawing destination Debug Line from: {}, {}, {}\n",
                           getName(),
                           posComp->position.x,
                           posComp->position.y,
                           posComp->position.z));

  _debugEntityId = _debugDrawUtils->addLine(posComp->position, direction);
  _drawn         = true;
}

int PlayerControl::movePlayerUnit(lua_State* L)
{
  std::int32_t n = lua_gettop(L);
  assert(n == 4);
  Entity* e = Scene::getSceneInstance().getEntityById(lua_tonumber(L, 1));
  assert(e != nullptr);
  Position* posComp = (Position*)e->getComponent(BitMasks::Components::POSITION);
  assert(posComp != nullptr);
  PlayerControl* me      = (PlayerControl*)lua_touserdata(L, lua_upvalueindex(1));
  me->_moveToDestination = glm::vec3(lua_tonumber(L, 2), lua_tonumber(L, 3), lua_tonumber(L, 4));
  return 0;
}
