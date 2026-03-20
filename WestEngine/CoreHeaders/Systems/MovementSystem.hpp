#pragma once

#include "../Components/Movement.hpp"
#include "../Components/Position.h"
#include "../Entity/World.hpp"
#include "../Interfaces/ISystem.h"

class MovementSystem : public ISystem
{
public:
  MovementSystem(std::shared_ptr<EventDispatcher> d, WestLogger* l, std::shared_ptr<ComponentRegistry> r );
  ~MovementSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(std::shared_ptr<World> w) override;

private:
  std::shared_ptr<World> _world;

  void moveToDestination(std::uint32_t id, Position* posComp, Movement* movComp);
  bool destinationReached(Position* posComp, Movement* movComp);
  void updatePosition(glm::vec3 local, Position* posComp, std::uint32_t id);
};
