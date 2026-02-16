#pragma once

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
};
