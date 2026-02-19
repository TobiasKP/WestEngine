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
  std::mutex _CameraMutex; 

  glm::vec3 _moveCamera        = glm::vec3(0.0f);
  std::atomic<bool> _cameraPending, _movementInitiated, _cleared;

  void updateCamera(glm::vec3 local); 

  static int movePlayerUnit(lua_State*);
  static int actionFinished(lua_State*);

  // Debug fields

  bool _camLog;
};
