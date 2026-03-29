#pragma once

#include "../Entity/Camera.h"
#include "../Entity/World.hpp"
#include "../Interfaces/ISystem.h"

#include <WestLogger.h>


class PlayerControl : public ISystem
{
public:
  PlayerControl(std::shared_ptr<EventDispatcher> d,
                WestLogger* l,
                std::shared_ptr<ComponentRegistry> r,
                std::shared_ptr<Camera> c);
  ~PlayerControl() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init(const std::shared_ptr<World>& w) override;
  void pollEvents() override;

  void passDestinationPosition(glm::vec3 dest);

private:
  bool isPlayerturn() const {
    return _state == 0;
  }

  std::shared_ptr<World> _world;
  std::shared_ptr<Camera> _cam;
  std::int32_t _tileIdx, _state;
};
