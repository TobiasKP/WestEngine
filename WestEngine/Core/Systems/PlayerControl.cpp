#include "../../CoreHeaders/Systems/PlayerControl.h"

#include "../../Constants/Systems.hpp"
#include "../Core/Scripting/LuaFacade.hpp"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <Config.h>
#include <format>

PlayerControl::PlayerControl(std::shared_ptr<EventDispatcher> d,
                             WestLogger* l,
                             std::shared_ptr<ComponentRegistry> r,
                             std::shared_ptr<Camera> c)
  : ISystem(d, l, r)
{
  _cam   = c;
  _state = 0;
  _busy  = false;
  setName(Systems::PLAYER_CONTROL);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("{} *** Initialized debug information\n", getName()));
#endif
};
PlayerControl::~PlayerControl() {}

void PlayerControl::init(const std::shared_ptr<World>& w)
{
  _world   = w;
  _tileIdx = 0;
  _me      = 0;
  _dispatcher->subscribe(EventIdentifiers::MOUSE_MOVE,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::MOUSE_LCLICK,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::MOUSE_RCLICK,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::GAME_EVENT,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::ACTION_FINISHED,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
}

void PlayerControl::update()
{
  if (_me == 0)
  {
    std::shared_ptr<ComponentArray<Control>> controlUnits = _reg->getComponentArray<Control>();
    assert(controlUnits->getComponents().size() > 0);
    auto& res = controlUnits->getComponents();
    _me = std::find_if(res.begin(), res.end(), [](const Control& c) { return c.active && !c.aiControl; })->entityId;
  }

  pollEvents();
  Movement* movComp = _reg->getComponent<Movement>(_me);
  assert(movComp != nullptr);
  if (!movComp->destination.has_value() && isPlayerturn())
  {
    Position* posComp = _reg->getComponent<Position>(_me);
    assert(posComp != nullptr);
    std::int32_t tileIdx          = _world->calculateIndex(posComp->position.x, posComp->position.z);
    std::int32_t dimension        = _world->getGridSize();
    std::int32_t column           = tileIdx % dimension;
    std::int32_t row              = tileIdx / dimension;
    std::vector<std::int32_t> res = _world->getReachableTiles(
      row, column, movComp->range, movComp->a, this, std::bind_front(&PlayerControl::isEnemy, this));
    movComp->reachableTiles = res;
  }
  else
  {
    _world->clearFlag(0x0002u);
  }
}

void PlayerControl::handleEvent(std::tuple<EventIdentifiers, EventPayload> event)
{
  switch (std::get<0>(event))
  {
    case EventIdentifiers::MOUSE_MOVE:
    {
      MousePayload* p         = std::get_if<MousePayload>(&std::get<1>(event));
      glm::vec3 hoverPosition = PositionCalculation::getWorldPosition(glm::vec2(p->x, p->y), _cam);
      _tileIdx                = _world->worldPosToTile(hoverPosition.x, hoverPosition.z);
      break;
    }
    case EventIdentifiers::MOUSE_RCLICK:
    {
      if (!isPlayerturn() || !_world->isVisible(_tileIdx))
      {
        break;
      }
      std::uint32_t id = _world->getEntityByIdx(_tileIdx);
      if (id > 0)
      {
        LuaFacade::getLuaFacadeInstance().emit(EventIdentifiers::ENTITY_RCLICK, EntityPayload{.entityId = id});
      }
      break;
    }
    case EventIdentifiers::MOUSE_LCLICK:
    {
      if (!isPlayerturn() || _busy || !_world->isVisible(_tileIdx))
      {
        break;
      }
      std::uint32_t id                     = _world->getEntityByIdx(_tileIdx);
      std::optional<glm::vec3> destination = _world->tileToWorldPos(_tileIdx);

      if (destination.has_value() && id == 0)
      {
        std::int32_t allowed = LuaFacade::getLuaFacadeInstance().getActionPoints(_me);
        if (allowed > 0)
        {
          passDestinationPosition(destination.value(), _me);
        }
      }
      else if (id != 0 && id != _me)
      {
        passAttackInformation(id);
      }

      break;
    }
    case EventIdentifiers::GAME_EVENT:
    {
      GamePayload* g = std::get_if<GamePayload>(&std::get<1>(event));
      _state         = g->turn;
      break;
    }
    case EventIdentifiers::ACTION_FINISHED:
    {
      ActionFinishedPayload* a = std::get_if<ActionFinishedPayload>(&std::get<1>(event));
      if (a->entityId == _me)
      {
        assert(_busy == true);
        _busy = false;
      }
      break;
    }
    default:
      break;
  }
}

void PlayerControl::updateDebuggingInfo() {}

void PlayerControl::passAttackInformation(std::uint32_t id)
{
  _busy = true;
  _dispatcher->dispatchEvent(EventIdentifiers::ATTACK_EVENT, AttackPayload{.attacker = _me, .target = id});
}

void PlayerControl::passDestinationPosition(glm::vec3 dest, std::uint32_t id)
{
  Movement* movComp = _reg->getComponent<Movement>(id);
  assert(movComp != nullptr);

  Position* posComp = _reg->getComponent<Position>(id);
  assert(posComp != nullptr);
  std::int32_t tile = _world->calculateIndex(dest.x, dest.z);
  bool inRange =
    std::find(movComp->reachableTiles.begin(), movComp->reachableTiles.end(), tile) != movComp->reachableTiles.end();
  std::vector<std::int32_t> path = _world->getPath(_world->calculateIndex(posComp->position.x, posComp->position.z),
                                                   tile,
                                                   movComp->range,
                                                   std::bind_front(&PlayerControl::isEnemy, this));
  if (inRange && !path.empty() && !movComp->destination.has_value())
  {
    bool result =
      LuaFacade::getLuaFacadeInstance().emit(EventIdentifiers::TILE_LCLICK, EntityPayload{.entityId = id});
    if (result)
    {
      _logger->log(
        Level::Error,
        std::format("{} *** Error emitting event: {}\n", getName(), eventName(EventIdentifiers::TILE_LCLICK)));
    }
    _busy = true;
    for (std::int32_t idx : path)
    {
      movComp->path.push_back(_world->tileToWorldPos(idx).value());
    }
    movComp->destination = movComp->path.front();
    movComp->path.pop_front();
  }
}

bool PlayerControl::isEnemy(std::uint32_t id)
{
  Control* c = _reg->getComponent<Control>(id);
  return c != nullptr && c->aiControl;
}
