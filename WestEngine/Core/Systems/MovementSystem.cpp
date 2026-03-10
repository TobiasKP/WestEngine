#include "../../CoreHeaders/Systems/MovementSystem.hpp"

#include "../../Constants/Systems.hpp"
#include "../CoreHeaders/Entity/Scene.h"

#include <format>

MovementSystem::MovementSystem(WestLogger* logger) : ISystem(), _logger(logger)
{
  setName(Systems::MOVEMENT);
};

MovementSystem::~MovementSystem() {}

void MovementSystem::init()
{
  _reg = Scene::getSceneInstance().getRegistry();
}

void MovementSystem::update()
{
  std::shared_ptr<ComponentArray<Movement>> movements = _reg->getComponentArray<Movement>();
  size_t size = movements->getSize(), current = 0;

  for (Movement& m : movements->getComponents())
  {
    if (current >= size)
    {
      break;
    }
    if (m.moving.load())
    {
      std::uint32_t id  = movements->getEntityIdByIdx(current);
      Position* posComp = _reg->getComponent<Position>(id);
      moveToDestination(id, posComp, &m);
      posComp->dirty.store(true);
      continue;
    }
    if (m.movementPending.exchange(false) && !m.moving.load())
    {
      std::uint32_t id = movements->getEntityIdByIdx(current);
      induceMovement(id, &m);
    }
    current++;
  }
}

void MovementSystem::moveToDestination(std::uint32_t id, Position* posComp, Movement* movComp)
{
  if (!destinationReached(posComp, movComp))
  {
    updatePosition(movComp->destination, posComp, id);
  }
  else
  {
    bool result = LuaFacade::getLuaFacadeInstance().onStateChange(id, LuaFacade::LuaStates::MOVING_FINISHED);
    if (result)
    {
      _logger->log(Level::Info,
                   std::format("{} *** Error calling lua function state change with state: {}\n",
                               getName(),
                               (std::int32_t)LuaFacade::LuaStates::IDLE));
    }
    movComp->moving.store(false);
#ifdef DEBUG
    movComp->debugInfoDisplayed = false;
    movComp->removeDebugInfo    = true;
#endif
  }
}

void MovementSystem::induceMovement(std::uint32_t id, Movement* movComp)
{
  bool result =
    LuaFacade::getLuaFacadeInstance().onTileClicked(id, LuaFacade::MouseAction::LMOUSE_CLICK, movComp->destination);
  if (result)
  {
    _logger->log(Level::Info,
                 std::format("{} *** Error calling lua function: {}\n",
                             getName(),
                             (std::int32_t)LuaFacade::MouseAction::LMOUSE_CLICK));
  }
  movComp->moving.store(true);
}

bool MovementSystem::destinationReached(Position* posComp, Movement* movComp)
{
  bool reached = glm::all(glm::epsilonEqual(posComp->position, movComp->destination, Config::GeneralConfig.EPSILON));
  return reached;
}

void MovementSystem::updatePosition(glm::vec3 local, Position* posComp, std::uint32_t id)
{
  assert(posComp != nullptr);
  glm::vec3 direction = local - posComp->position;

  if (glm::length2(direction) <= 0.025f)
  {
    posComp->position = local;
  }
  else
  {
    direction          = glm::normalize(direction) * Config::GeneralConfig.SPEED * Config::GeneralConfig.DELTA;
    posComp->position += direction;
  }
  Scene::getSceneInstance().getWorld()->addEntityIdToIdx(posComp->position.x, posComp->position.z, id);
}

void MovementSystem::updateDebuggingInfo() {}
