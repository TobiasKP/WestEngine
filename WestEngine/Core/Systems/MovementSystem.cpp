#include "../../CoreHeaders/Systems/MovementSystem.hpp"

#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Utils/Math/PositionCalculation.h"
#include "../Core/Scripting/LuaFacade.hpp"

#include <format>

MovementSystem::MovementSystem(std::shared_ptr<EventDispatcher> d, WestLogger* l, std::shared_ptr<ComponentRegistry> r)
  : ISystem(d, l, r)
{
  setName(Systems::MOVEMENT);
};

MovementSystem::~MovementSystem() {}

void MovementSystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
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

    std::uint32_t id = movements->getEntityIdByIdx(current);
    if (m.destination.has_value())
    {
      Position* posComp = _reg->getComponent<Position>(id);
      moveToDestination(id, posComp, &m);
      posComp->dirty.store(true);
      continue;
    }
    current++;
  }
}

void MovementSystem::moveToDestination(std::uint32_t id, Position* posComp, Movement* movComp)
{
  if (!destinationReached(posComp, movComp))
  {
    updatePosition(*movComp->destination, posComp, id);
  }
  else if (!movComp->path.empty())
  {
    movComp->destination = movComp->path.front();
    movComp->path.pop_front();
    updatePosition(*movComp->destination, posComp, id);
  }
  else
  {
    bool result = LuaFacade::getLuaFacadeInstance().emit(
      EventIdentifiers::ENTITY_STATE_CHANGE,
      StateChangePayload{.entityId = id, .oldState = LuaFacade::MOVING, .newState = LuaFacade::IDLE});
    if (result)
    {
      _logger->log(Level::Error,
                   std::format("{} *** Error emitting state change to state: {}\n",
                               getName(),
                               (std::int32_t)LuaFacade::LuaStates::IDLE));
    }
    _dispatcher->dispatchEvent(EventIdentifiers::ACTION_FINISHED, ActionFinishedPayload{.entityId = id});
    movComp->destination.reset();
#ifdef DEBUG
    movComp->debugInfoDisplayed = false;
    movComp->removeDebugInfo    = true;
#endif
  }
}

bool MovementSystem::destinationReached(Position* posComp, Movement* movComp)
{
  bool reached = glm::all(glm::epsilonEqual(posComp->position, *movComp->destination, Config::GeneralConfig.EPSILON));
  return reached;
}

void MovementSystem::updatePosition(glm::vec3 local, Position* posComp, std::uint32_t id)
{
  assert(posComp != nullptr);
  PositionCalculation::updatePosition(local, posComp);
  std::uint32_t occupant = _world->getEntityByIdx(_world->calculateIndex(posComp->position.x, posComp->position.z));
  if (occupant == 0 || occupant == id)
  {
    _world->updateEntityIdToIdx(posComp->position.x, posComp->position.z, id);
  }
}

void MovementSystem::updateDebuggingInfo() {}
