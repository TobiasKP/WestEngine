#pragma once

#include "../Entity/Scene.h"
#include "../Interfaces/ISystem.h"

class ProjectileSystem : public ISystem
{
public:
  ProjectileSystem(std::shared_ptr<EventDispatcher> d,
                   WestLogger* l,
                   std::shared_ptr<ComponentRegistry> r,
                   std::shared_ptr<Scene> s);
  ~ProjectileSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;

  static int spawnProjectile(lua_State*);

protected:
  void handleEvent(std::tuple<EventIdentifiers, EventPayload> event) override;

private:
  void travel(std::uint32_t id, Position* posComp, Projectile* p);

  std::vector<std::uint32_t> _toRemove;
  std::shared_ptr<World> _world;
  std::shared_ptr<Scene> _scene;
};
