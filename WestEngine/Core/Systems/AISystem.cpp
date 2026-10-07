#include "../../CoreHeaders/Systems/AISystem.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../../CoreHeaders/Components/Faction.hpp"
#include "../Scripting/LuaFacade.hpp"

#include <format>
#include <iostream>

AISystem::AISystem(std::shared_ptr<EventDispatcher> d, WestLogger* l, std::shared_ptr<ComponentRegistry> r)
  : ISystem(d, l, r)
{
  _facade   = &LuaFacade::getLuaFacadeInstance();
  _registry = r;
  _state    = 0;
  _me       = 0;
  _busy     = false;
}

AISystem::~AISystem() {}

void AISystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
  _facade->registerCFunction(gatherWorldInformation, LuaAPI::C_GET_WORLD_INFO.data(), this);
  _facade->registerCFunction(aiMoveCommand, LuaAPI::C_AI_MOVE.data(), this);
  _facade->registerCFunction(aiAttackCommand, LuaAPI::C_AI_ATTACK.data(), this);
  _facade->registerCFunction(aiEndAction, LuaAPI::C_AI_END.data(), this);
  _dispatcher->subscribe(EventIdentifiers::GAME_EVENT,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
  _dispatcher->subscribe(EventIdentifiers::ACTION_FINISHED,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
}

void AISystem::update()
{
  pollEvents();
  if (!isAIturn())
  {
    return;
  }

  std::shared_ptr<ComponentArray<Control>> controlUnits = _reg->getComponentArray<Control>();
  assert(controlUnits->getComponents().size() > 0); 
  if (_me == 0)
  {
    size_t size = controlUnits->getSize();

    for (size_t i = 0; i < size; i++)
    {
      Control* c = controlUnits->getComponentByIdx(i);

      if (c->aiControl && std::find(_handled.begin(), _handled.end(), c->entityId) == _handled.end())
      {
        _me = c->entityId;
        break;
      }
    }
  }
  if (!_busy && _me != 0)
  {
    _busy = true;
    _facade->emit(EventIdentifiers::AI_THINK, EntityPayload{.entityId = _me});
  }
  else if (_me == 0 && !_busy)
  {
    _dispatcher->dispatchEvent(EventIdentifiers::GAME_EVENT, GamePayload{.turn = !_state});
  }
}

void AISystem::updateDebuggingInfo() {};

void AISystem::handleEvent(std::tuple<EventIdentifiers, EventPayload> event)
{
  switch (std::get<0>(event))
  {
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

int AISystem::gatherWorldInformation(lua_State* L)
{
  AISystem* me      = (AISystem*)lua_touserdata(L, lua_upvalueindex(1));
  Position* posComp = me->_reg->getComponent<Position>(me->_me);
  Movement* movComp = me->_reg->getComponent<Movement>(me->_me);
  Equipment* eqComp = me->_reg->getComponent<Equipment>(me->_me);
  assert(posComp != nullptr);
  assert(movComp != nullptr);
  assert(eqComp != nullptr);
  Weapon* active = nullptr;
  if (eqComp->active == 1)
  {
    active = &eqComp->primary;
  }
  else if (eqComp->active == 2)
  {
    active = &eqComp->secondary;
  }
  std::int32_t tileIdx            = me->_world->calculateIndex(posComp->position.x, posComp->position.z);
  std::int32_t dimension          = me->_world->getGridSize();
  std::int32_t column             = tileIdx % dimension;
  std::int32_t row                = tileIdx / dimension;
  std::vector<std::int32_t> tiles = me->_world->getReachableTiles(
    row, column, movComp->range, movComp->a, me, std::bind_front(isHostile, std::ref(*me->_reg), me->_me));
  tiles.erase(std::remove(tiles.begin(), tiles.end(), tileIdx), tiles.end());
  std::vector<std::int32_t> entities;
  if (active)
  {
    std::int32_t sight = getLineOfSightRange(me->_reg->getComponent<LineOfSight>(me->_me), movComp).value();
    std::int32_t range = std::min(static_cast<std::int32_t>(active->range), sight);
    entities           = me->_world->getEntitiesInRange(row, column, range, movComp->a, me->_me);
  }
  else
  {
    me->_logger->log(Level::Error,
                     std::format("AISystem ### entity {} has no active weapon (equipment slot {})\n",
                                 me->_me,
                                 eqComp->active));
  }
  lua_createtable(L, 0, 2);
  lua_createtable(L, tiles.size(), 0);
  for (std::int32_t i = 0; i < tiles.size(); i++)
  {
    std::int32_t tile = tiles[i];
    lua_pushinteger(L, tile);
    lua_seti(L, -2, i + 1);
  }
  lua_setfield(L, -2, "TilesInRange");
  lua_createtable(L, entities.size(), 0);
  std::cout << entities.size() << std::endl;
  for (std::int32_t i = 0; i < entities.size(); i++)
  {
    std::int32_t entityTile = entities[i];
    lua_pushinteger(L, entityTile);
    lua_seti(L, -2, i + 1);
  }
  lua_setfield(L, -2, "EnemiesInRange");

  return 1;
}

int AISystem::aiMoveCommand(lua_State* L)
{
  AISystem* me      = (AISystem*)lua_touserdata(L, lua_upvalueindex(1));
  std::int32_t id   = lua_tointeger(L, 1);
  Movement* movComp = me->_reg->getComponent<Movement>(id);
  Position* posComp = me->_reg->getComponent<Position>(id);
  assert(movComp != nullptr);
  assert(posComp != nullptr);
  std::int32_t start             = me->_world->calculateIndex(posComp->position.x, posComp->position.z);
  std::int32_t target            = lua_tointeger(L, 2);
  std::vector<std::int32_t> path = me->_world->getPath(
    start, target, movComp->range, std::bind_front(isHostile, std::ref(*me->_reg), id));
  if (path.empty())
  {
    me->_logger->log(
      Level::Error, std::format("AISystem ### entity {} has no path from tile {} to {}\n", id, start, target));
    lua_pushboolean(L, false);
    return 1;
  }
  me->_busy = true;
  me->_world->followPath(movComp, path);
  lua_pushboolean(L, true);
  return 1;
}

int AISystem::aiAttackCommand(lua_State* L)
{
  AISystem* me          = (AISystem*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint32_t id      = lua_tointeger(L, 1);
  std::int32_t enemyIdx = lua_tointeger(L, 2);
  std::uint32_t enemyId = me->_world->getEntityByIdx(enemyIdx);
  assert(id > 0 && enemyId > 0);
  me->_busy = true;
  me->_dispatcher->dispatchEvent(EventIdentifiers::ATTACK_EVENT, AttackPayload{.attacker = id, .target = enemyId});
  return 0;
}

int AISystem::aiEndAction(lua_State* L)
{
  AISystem* me = (AISystem*)lua_touserdata(L, lua_upvalueindex(1));
  bool endturn = lua_toboolean(L, 1);
  if (endturn)
  {
    me->_me = 0;
    me->_handled.clear();
  }
  else
  {
    me->_handled.push_back(me->_me);
    me->_me = 0;
  }
  me->_busy = false;
  return 0;
}
