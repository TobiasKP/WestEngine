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
  if (p == nullptr)
  {
    return;
  }
  const Position* pos = _reg->getComponent<Position>(p->destination);
  if (pos == nullptr)
  {
    _toRemove.push_back(id);
  }

  glm::vec3 targetPos = pos->position;
  bool reached        = glm::all(glm::epsilonEqual(posComp->position, targetPos, Config::GeneralConfig.EPSILON));
  if (reached)
  {
    if (p->hit)
    {
      Health* health   = _reg->getComponent<Health>(p->destination);
      health->current -= p->damage;

      std::string newHealth = std::to_string(health->current);
      _dispatcher->dispatchEvent(EventIdentifiers::INTERFACE_UPDATE,
                                 InterfacePayload{.event = 0x10, .entityId = p->destination, .newValue = newHealth});
      _dispatcher->dispatchEvent(EventIdentifiers::INTERFACE_UPDATE,
                                 InterfacePayload{.event = 0x08, .entityId = p->destination, .newValue = newHealth});
      if (health->current <= 0)
      {
        _toRemove.push_back(p->destination);
      }
    }
    _toRemove.push_back(id);
    _dispatcher->dispatchEvent(EventIdentifiers::ACTION_FINISHED, ActionFinishedPayload{.entityId = p->owner});
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
      Weapon* active = e->activeWeapon();
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
      const EntityAttackPayload attack{.attacker   = a->attacker,
                                       .target     = a->target,
                                       .bulletType = active->bulletType,
                                       .range      = active->range,
                                       .distance   = static_cast<std::uint32_t>(std::max(dx, dz)),
                                       .damage     = active->dmg,
                                       .accuracy   = active->accuracy,
                                       .spawnX     = aPosComp->position.x,
                                       .spawnY     = aPosComp->position.z};
      LuaFacade::getLuaFacadeInstance().emit(EventIdentifiers::ENTITY_ATTACK, attack);
      break;
    }
    default:
      break;
  }
}
