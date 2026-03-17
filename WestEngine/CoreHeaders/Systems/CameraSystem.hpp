#pragma once

#include "../../CoreHeaders/Entity/Camera.h"
#include "../Interfaces/ISystem.h"

class CameraSystem : public ISystem
{
public:
  CameraSystem() : ISystem() {};
  CameraSystem(WestLogger* logger, std::shared_ptr<EventDispatcher> d);
  ~CameraSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init() override;
  void pollEvents() override;

 private:
  bool _dirty;
  WestLogger* _logger;
  std::shared_ptr<EventDispatcher> _dispatcher;
  Camera* _cam;
  glm::vec3 _move;
};
