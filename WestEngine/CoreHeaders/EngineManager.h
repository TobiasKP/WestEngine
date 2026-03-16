#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "Interfaces/IManager.h"
#include "WindowManager.h"

#include <array>
#include <Config.h>

class EngineManager : public IManager
{
public:
  EngineManager();
  EngineManager(WestLogger* logger);
  ~EngineManager();

  // Getter
  inline bool shouldExit()
  {
    return _exitEngine;
  }
  inline std::int32_t getFps()
  {
    return _fps;
  }

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

protected:
  inline void setFps(std::int32_t fps)
  {
    _fps = fps;
  }

private:
  enum CYCLE { STARTUP, INIT, UPDATE, LOAD, PAUSE };

  bool _exitEngine;
  std::int32_t _fps;
  std::array<IManager*, CoreConstants::MAX_Q_SIZE> _manager;
  WindowManager* _windowManager;
  float _FRAMERATE;
  float _FRAMETIME;
  const long _NANOSECOND = 1000000000;
  std::shared_ptr<EventDispatcher> _dispatcher;

  std::int32_t iterateQ(CYCLE code);
  std::int32_t executeCycle(CYCLE code, IManager& item);
  std::int32_t initializeSettings();
  std::int32_t fillSettings(std::string key, std::string value);
  bool isPauseCycle(CYCLE code, IManager& item);
};
