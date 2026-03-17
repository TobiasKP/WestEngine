#include "../../CoreHeaders/Systems/PlayerControl.h"

#include "../../Constants/Systems.hpp"
#include "../CoreHeaders/Entity/Scene.h"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <Config.h>
#include <format>

PlayerControl::PlayerControl(WestLogger* logger, std::shared_ptr<EventDispatcher> d) : ISystem()
{
  setName(Systems::PLAYER_CONTROL);
  _logger     = logger;
  _dispatcher = d;
#ifdef DEBUG
  _logger->log(Level::Info, std::format("{} *** Initialized debug information\n", getName()));
#endif
};

PlayerControl::~PlayerControl() {}

void PlayerControl::init()
{
  _tileIdx = 0;
  _reg     = Scene::getSceneInstance().getRegistry();
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::MOUSE_LCLICK,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::MOUSE_RCLICK,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
}

void PlayerControl::update()
{
  pollEvents();
  std::shared_ptr<ComponentArray<Control>> controlUnits = _reg->getComponentArray<Control>();
  std::uint32_t id                                      = controlUnits->getComponents()[0].entityId;
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

void PlayerControl::pollEvents()
{
  std::vector<std::tuple<EventIdentifiers, EventPayload>> events = _eventQueue.drain();
  World* w                                                       = Scene::getSceneInstance().getWorld();
  for (std::tuple<EventIdentifiers, EventPayload> event : events)
  {
    switch (std::get<0>(event))
    {
      case EventIdentifiers::MOUSE_MOVE:
      {
        MousePayload* p = std::get_if<MousePayload>(&std::get<1>(event));
        glm::vec3 hoverPosition =
          PositionCalculation::getWorldPosition(glm::vec2(p->x, p->y), Scene::getSceneInstance().getCamera());
        _tileIdx = Scene::getSceneInstance().getWorld()->worldPosToTile(hoverPosition.x, hoverPosition.z);
        break;
      }
      case EventIdentifiers::MOUSE_RCLICK:
      {
        std::uint32_t id = w->getEntityByIdx(_tileIdx);
        if (id > 0)
        {
          LuaFacade::getLuaFacadeInstance().onEntityClicked(1, LuaFacade::MouseAction::RMOUSE_CLICK, id);
        }
      }
      case EventIdentifiers::MOUSE_LCLICK:
      {
        std::uint32_t id                     = w->getEntityByIdx(_tileIdx);
        std::optional<glm::vec3> destination = w->tileToWorldPos(_tileIdx);
        if (destination.has_value() && id == 0)
        {
          passDestinationPosition(destination.value());
        }
        break;
      }
      default:
        break;
    }
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
