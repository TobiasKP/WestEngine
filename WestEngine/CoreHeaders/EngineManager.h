#pragma once

#include "Interfaces/IManager.h"
#include "WindowManager.h"

#include <Config.h>
#include <array>
#include <memory>

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
  const long _NANOSECOND = 1000000000;
  const float _FRAMERATE = Config::GeneralConfig.FPS;
  const float _FRAMETIME = 1.0f / _FRAMERATE;

  std::int32_t iterateQ(CYCLE code);
  std::int32_t executeCycle(CYCLE code, IManager& item);
  std::int32_t initializeSettings();
  std::int32_t fillSettings(std::string key, std::string value);
  bool isPauseCycle(CYCLE code, IManager& item);
};
