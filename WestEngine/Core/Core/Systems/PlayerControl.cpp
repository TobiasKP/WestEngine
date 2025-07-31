#include "../../CoreHeaders/Systems/PlayerControl.h"

#include <format>

#include "../../Config/Config.h"
#include "../../Constants/Systems.h"

PlayerControl::PlayerControl(WestLogger *logger)
    : ISystem(), _logger(logger), _cameraPending(false) {
  setName(Systems::PLAYER_CONTROL);
#ifdef DEBUG
  _logger->writeInfo(std::format("{} *** Initialized debug information", getName()));
  _debugDrawUtils = new DebugDrawUtils(_logger);
  _debugEntity = nullptr;
  _drawn = false, _camLog = true, _posLog = true;
#endif
};

PlayerControl::~PlayerControl() {}

void PlayerControl::update() {
  assert(_logger != nullptr);
  if (_cameraPending.exchange(false)) {
    glm::vec3 localCam;
    {
      std::lock_guard<std::mutex> lock(_CameraMutex);
      localCam = _moveCamera;
    }

#ifdef DEBUG
    if (_camLog) {
      _logger->writeInfo(
          std::format("{} *** updating Camera position ({}, {}, {})\n",
                      getName(), localCam.x, localCam.y, localCam.z));
      _camLog = false;
    }
#endif
    updateCamera(localCam);
  }
#ifdef DEBUG
  else {
    _camLog = true;
  }
#endif

  bool pending = _movementPending.exchange(false);
  Position *posComp = nullptr;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    posComp = (Position *)getEntities().front()->getComponent(
        BitMasks::Components::POSITION);
  }
  assert(posComp != nullptr);
  if (pending || !destinationReached(posComp)) {
    glm::vec3 localDest;
#ifdef DEBUG
    if (_debugEntity != nullptr && !_debugEntity->isDestroyed() && pending) { 
      _logger->writeInfo(std::format("{} *** Destroying destination Debug Line\n", getName()));
      _debugEntity->destroy();
      _drawn = false;
    }
#endif
    {
      std::lock_guard<std::mutex> lock(_MovementMutex);
      localDest = _moveToDestination;
    }
#ifdef DEBUG
    if (_posLog) {
      glm::vec3 currentPos = posComp->position; 
      _logger->writeInfo(std::format("{} *** Moving entity at position: ({}, {}, {}) - to "
                      "position: ({}, {}, {})\n",
                      getName(), currentPos.x, currentPos.y, currentPos.z,
                      _moveToDestination.x, _moveToDestination.y,
                      _moveToDestination.z));
      _posLog = false;
    }
#endif
    updatePosition(localDest, posComp);
  }

#ifdef DEBUG
  else {
    _posLog = true;
  }
#endif
}

void PlayerControl::updateCamera(glm::vec3 local) {
  Camera *camera = Scene::getSceneInstance().getCamera();
  assert(camera != nullptr);
  camera->movePosition(local.x, local.y, local.z);
}

void PlayerControl::updatePosition(glm::vec3 local, Position *posComp) {
  assert(posComp != nullptr);
  glm::vec3 direction = local - posComp->position;

  if (glm::length2(direction) <=
      Config::GeneralConfig.SPEED * Config::GeneralConfig.SPEED) {
    posComp->position = local;
    return;
  }

  direction = glm::normalize(direction) * Config::GeneralConfig.SPEED;
  posComp->position += direction;
}

bool PlayerControl::destinationReached(Position *posComp) {
  bool reached = glm::all(glm::epsilonEqual(
      posComp->position, _moveToDestination, Config::GeneralConfig.EPSILON));
#ifdef DEBUG
  if (_debugEntity != nullptr && !_debugEntity->isDestroyed() && reached) { 
    _logger->writeInfo(std::format("{} *** Destroying destination Debug Line\n", getName()));
    _debugEntity->destroy();
    _drawn = false;
  }
#endif
  return reached;
}

void PlayerControl::setCameraMovement(glm::vec3 move) {
  std::unique_lock<std::mutex> lock(_CameraMutex, std::try_to_lock);
  if (lock.owns_lock()) {
    _moveCamera = move;
    _cameraPending.store(true);
  }
}

void PlayerControl::setDestinationPosition(glm::vec3 dest) {
  std::unique_lock<std::mutex> lock(_MovementMutex, std::try_to_lock);
  if (lock.owns_lock()) {
    _moveToDestination = dest;
    _movementPending.store(true);
  }
}

void PlayerControl::updateDebuggingInfo() {
  if (_debugEntity != nullptr && _debugEntity->isDestroyed()) {
    _debugDrawUtils->unloadModel(_debugEntity);
    _debugEntity = nullptr;
  }

  if (_drawn)
    return;

  Position *posComp = (Position *)getEntities().front()->getComponent(
      BitMasks::Components::POSITION);
  assert(posComp != nullptr);
  glm::vec3 direction = _moveToDestination - posComp->position;
  if (glm::length2(direction) <=
      Config::GeneralConfig.SPEED * Config::GeneralConfig.SPEED) {
    return;
  }
 
  _logger->writeInfo(std::format("{} *** Drawing destination Debug Line from: {}, {}, {}\n",
                  getName(), posComp->position.x, posComp->position.y,
                  posComp->position.z));

  glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
  _debugEntity = _debugDrawUtils->addLine(posComp->position, direction, color);
  _drawn = true;
}
