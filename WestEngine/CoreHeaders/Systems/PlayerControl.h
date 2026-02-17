#pragma once

#include "../Components/Position.h"
#include "../Interfaces/ISystem.h"

#include <atomic>
#include <mutex>
#include <WestLogger.h>


class PlayerControl : public ISystem
{
public:
  PlayerControl() : ISystem() {};
  PlayerControl(WestLogger* logger);
  ~PlayerControl() override;

  void update() override;
  void updateDebuggingInfo() override;
  void init() override;
  void setCameraMovement(glm::vec3 move);
  void passDestinationPosition(glm::vec3 dest);

private:
  WestLogger* _logger;
  std::mutex _CameraMutex, _MovementMutex;

  glm::vec3 _moveToDestination = glm::vec3(0.0f);
  glm::vec3 _moveCamera        = glm::vec3(0.0f);
  std::atomic<bool> _cameraPending, _movementInitiated;

  void updateCamera(glm::vec3 local);
  void updatePosition(glm::vec3 local, Position* posComp);

  static int movePlayerUnit(lua_State*);
  static int actionFinished(lua_State*);

  // Debug fields

  bool _camLog;
};
