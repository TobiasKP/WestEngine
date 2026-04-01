#pragma once

#include "../../CoreHeaders/Entity/Camera.h"
#include "../Interfaces/ISystem.h"

class CameraSystem : public ISystem
{
public:
  CameraSystem(std::shared_ptr<EventDispatcher> d,
               WestLogger* l,
               std::shared_ptr<ComponentRegistry> r,
               std::shared_ptr<Camera> c);
  ~CameraSystem() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;

protected:
  void handleEvent(std::tuple<EventIdentifiers, EventPayload> event) override;

private:
  bool _dirty;
  std::shared_ptr<Camera> _cam;
  glm::vec3 _move, _rot;
};
