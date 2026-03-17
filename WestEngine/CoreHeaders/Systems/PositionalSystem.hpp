#pragma once

#include "../Interfaces/ISystem.h"

class PositionalSystem : public ISystem
{
public:
  PositionalSystem() : ISystem() {};
  PositionalSystem(WestLogger* logger, std::shared_ptr<EventDispatcher> d);
  ~PositionalSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init() override;
  void pollEvents() override;

private:
  WestLogger* _logger;
  std::shared_ptr<EventDispatcher> _dispatcher;
  std::int32_t _tileIdx, _lastEntity;
};
