#pragma once

#include "../Entity/World.hpp"
#include "../Interfaces/ISystem.h"

class VisibilitySystem : public ISystem
{
public:
  VisibilitySystem(std::shared_ptr<EventDispatcher> d, WestLogger* l, std::shared_ptr<ComponentRegistry> r);
  ~VisibilitySystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;

private:
  std::shared_ptr<World> _world;
  std::vector<std::pair<std::int32_t, std::int32_t>> _origins;
};
