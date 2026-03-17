#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "Entity/Scene.h"
#include "Interfaces/IManager.h"
#include "Interfaces/ISystem.h"

#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class SystemManager : public IManager
{
public:
  SystemManager();
  SystemManager(WestLogger* logger, std::shared_ptr<EventDispatcher> d);
  ~SystemManager();

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

  static ISystem* getSystemByName(const std::string_view name);

private:
  Scene* _scene;
  static std::array<ISystem*, 4> _systems;
  std::shared_ptr<EventDispatcher> _dispatcher;

#ifdef DEBUG
  std::int32_t _loggingFrequence = 0;
  double _avgTime                = 0;
#endif
};
