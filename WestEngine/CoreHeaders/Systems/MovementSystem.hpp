#pragma once

#include "../Components/Movement.hpp"
#include "../Components/Position.h"
#include "../Interfaces/ISystem.h"

class MovementSystem : public ISystem
{
public:
  MovementSystem() : ISystem() {};
  MovementSystem(WestLogger* logger);
  ~MovementSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init() override;

private: 
  void moveToDestination(std::uint32_t id, Position* posComp, Movement* movComp);
  bool destinationReached(Position* posComp, Movement* movComp);
  void updatePosition(glm::vec3 local, Position* posComp, std::uint32_t id);
};
