#pragma once

#include <mutex>
#include <vector>

#include "Camera.h"
#include "Entity.h"

#include "../../Constants/CoreConstants.h"

class Scene {
public:
  static Scene &getSceneInstance();

  Scene(Scene const &) = delete;
  void operator=(Scene const &) = delete;

  // Functions
  void init();
  void addEntity(Entity *entity);
  void addDebugEntity(Entity *entity);
  void addCamera(Camera *cam);
  void removeEntity(Entity *entity);
  void deleteScene();

  // Getter
  std::vector<Entity *> getEntities();
  inline std::string getSceneName() { return _sceneName; }
  inline Camera *getCamera() { return _camera; }

private:
  static Scene _sceneInstance;
  static std::mutex _mutex;

  static Camera *_camera;
  std::vector<Entity *> _entities;
  std::vector<Entity *> _debugEntities;
  std::string _sceneName = CoreConstants::UNDEFINED_STRING;

  Scene();
  ~Scene();
};
