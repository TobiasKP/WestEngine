#pragma once

#include "Interfaces/IManager.h"

#include "DataStructure/WestQ.h"
#include "WindowManager.h"

class EngineManager : public IManager {

public:
  EngineManager();
  EngineManager(WestLogger *logger);
  ~EngineManager();

  // Getter
  inline bool shouldExit() { return _exitEngine; }
  inline std::int32_t getFps() { return _fps; }

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

protected:
  inline void setFps(std::int32_t fps) { _fps = fps; }

private:
  enum CYCLE { STARTUP, INIT, UPDATE, LOAD, PAUSE };

  bool _exitEngine;
  std::int32_t _fps;
  WestQ *_engineQ;
  WindowManager *_windowManager;
  const long _NANOSECOND = 1000000000;
  const float _FRAMERATE = 30.0f;
  const float _FRAMETIME = 1.0f / _FRAMERATE;

  std::int32_t iterateQ(CYCLE code);
  std::int32_t executeCycle(CYCLE code, IManager *item);
  bool isPauseCycle(CYCLE code, IManager *item);
};
