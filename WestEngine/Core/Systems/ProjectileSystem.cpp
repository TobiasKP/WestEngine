#include "../../CoreHeaders/Systems/ProjectileSystem.hpp"

#include "../../CoreHeaders/Utils/Math/PositionCalculation.h"
#include "../Core/Scripting/LuaFacade.hpp"

#include <glm/gtc/epsilon.hpp>
#include <WestInterfaceFacade.h>

using namespace WestInterface;

ProjectileSystem::ProjectileSystem(std::shared_ptr<EventDispatcher> d,
                                   WestLogger* l,
                                   std::shared_ptr<ComponentRegistry> r,
                                   std::shared_ptr<Scene> s)
  : ISystem(d, l, r)
{
  _scene = s;
};

ProjectileSystem::~ProjectileSystem() {}

void ProjectileSystem::update()
{
  pollEvents();
  std::shared_ptr<ComponentArray<Projectile>> projectiles = _reg->getComponentArray<Projectile>();
  size_t size = projectiles->getSize(), current = 0;
  for (Projectile& p : projectiles->getComponents())
  {
    if (current >= size)
    {
      break;
    }
    std::uint32_t id  = projectiles->getEntityIdByIdx(current);
    Position* posComp = _reg->getComponent<Position>(id);
    travel(id, posComp, &p);
    posComp->dirty.store(true);
    current++;
  }
  for (std::uint32_t id : _toRemove)
  {
    Entity* e = _scene->getEntityById(id);
    assert(e != nullptr);
    e->destroy();
  }
  _toRemove.clear();
}

void ProjectileSystem::travel(std::uint32_t id, Position* posComp, Projectile* p)
{
  glm::vec3 targetPos = _reg->getComponent<Position>(p->destination)->position;
  bool reached        = glm::all(glm::epsilonEqual(posComp->position, targetPos, Config::GeneralConfig.EPSILON));
  if (reached)
  {
    if (p->hit)
    {
      Health* health      = _reg->getComponent<Health>(p->destination);
      health->current    -= p->damage;
      InterfacePayload i  = {};
      i.event             = 0x10;
      i.entityId          = p->destination;
      i.newValue          = std::to_string(health->current);
      _dispatcher->dispatchEvent(EventIdentifiers::INTERFACE_UPDATE, i);
      if (health->current <= 0)
      {
        _toRemove.push_back(p->destination);
      }
    }
    _toRemove.push_back(id);
  }
  else
  {
    PositionCalculation::updatePosition(targetPos, posComp, p->speed);
  }
}

void ProjectileSystem::updateDebuggingInfo() {}

void ProjectileSystem::init(const std::shared_ptr<World>& w)
{
  _world = w;
  _dispatcher->subscribe(EventIdentifiers::ATTACK_EVENT,
                         [this](EventIdentifiers event, EventPayload payload) { pushEvent(event, payload); });
}

void ProjectileSystem::handleEvent(std::tuple<EventIdentifiers, EventPayload> event)
{
  switch (std::get<0>(event))
  {
    case EventIdentifiers::ATTACK_EVENT:
    {
      AttackPayload* a = std::get_if<AttackPayload>(&std::get<1>(event));
      assert(a != nullptr);
      Equipment* e = _reg->getComponent<Equipment>(a->attacker);
      assert(e != nullptr);
      Weapon* active = nullptr;
      if (e->active == 1)
      {
        active = &e->primary;
      }
      else if (e->active == 2)
      {
        active = &e->secondary;
      }
      if (!active)
      {
        _logger->log(Level::Error,
                     std::format("{} *** Entity: {} has no active weaponary to attack.", getName(), a->attacker));
        break;
      }

      Position* aPosComp     = _reg->getComponent<Position>(a->attacker);
      Position* tPosComp     = _reg->getComponent<Position>(a->target);
      std::int32_t aTile     = _world->calculateIndex(aPosComp->position.x, aPosComp->position.z);
      std::int32_t tTile     = _world->calculateIndex(tPosComp->position.x, tPosComp->position.z);
      std::int32_t dimension = _world->getGridSize();
      std::int32_t dx        = std::abs(aTile % dimension - tTile % dimension);
      std::int32_t dz        = std::abs(aTile / dimension - tTile / dimension);
      LuaFacade::getLuaFacadeInstance().onAttack(a->attacker, 
                                                 active->bulletType,
                                                 active->range,
                                                 std::max(dx, dz),
                                                 active->dmg,
                                                 active->accuracy,
                                                 a->target,
                                                 aPosComp->position.x,
                                                 aPosComp->position.z);
      break;
    }
    default:
      break;
  }
}
