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

  void passDestinationPosition(glm::vec3 dest, std::uint32_t id);
  void passAttackInformation(std::uint32_t id);


protected:
  void handleEvent(std::tuple<EventIdentifiers, EventPayload> event) override;


private:
  bool isPlayerturn() const {
    return _state == 0;
  }
  bool isEnemy(std::uint32_t id);

  std::shared_ptr<World> _world;
  std::shared_ptr<Camera> _cam;
  std::int32_t _tileIdx, _state;
  std::uint32_t _me;
  bool _busy;
};
