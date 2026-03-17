#pragma once

#include "../Interfaces/ISystem.h"

#include <WestLogger.h>


class PlayerControl : public ISystem
{
public:
  PlayerControl() : ISystem() {};
  PlayerControl(WestLogger* logger, std::shared_ptr<EventDispatcher> d);
  ~PlayerControl() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init() override;
  void pollEvents() override;

  void passDestinationPosition(glm::vec3 dest);

private: 
  std::int32_t _tileIdx;
};
