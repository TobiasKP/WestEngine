#pragma once

#include "Interfaces/IManager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>

#include "Entity/Scene.h"
#include "Interfaces/ISystem.h"

class SystemManager : public IManager {

public:
  SystemManager();
  SystemManager(WestLogger *logger);
  ~SystemManager();

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  static ISystem* getSystemByName(const std::string name);

private:
  Scene *_scene;
  static std::array<ISystem *, 1> _systems;

#ifdef DEBUG
  std::int32_t _loggingFrequence = 0;
  double _avgTime = 0;
#endif
};
