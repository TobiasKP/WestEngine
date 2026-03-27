#pragma once

#include "../Entity/Camera.h"
#include "../Entity/World.hpp"
#include "../Interfaces/ISystem.h"

class PositionalSystem : public ISystem
{
public:
  PositionalSystem(std::shared_ptr<EventDispatcher> d,
                   WestLogger* l,
                   std::shared_ptr<ComponentRegistry> r,
                   std::shared_ptr<Camera> c);
  ~PositionalSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;
  void pollEvents() override;

private:
  std::shared_ptr<World> _world;
  std::shared_ptr<Camera> _cam;
  std::int32_t _tileIdx, _lastEntity;
  bool _highlighted;
  glm::vec3 _lastEmissive;
};
