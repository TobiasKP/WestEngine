#pragma once

#include "../../Constants/CoreConstants.hpp"
#include "Camera.h"
#include "Entity.h"

#include <mutex>
#include <vector>

class Scene
{
public:
  static Scene& getSceneInstance();

  Scene(Scene const&)          = delete;
  void operator=(Scene const&) = delete;

  // Functions
  void init();
  void addEntity(Entity&& entity);
  void addDebugEntity(Entity&& entity);
  void addCamera(Camera* cam);
  void removeEntity(const Entity& entity);
  void deleteScene();

  // Getter
  std::vector<Entity> getEntities();
  //TODO: register ID in hashmap saving index in vector to have faster access if a specific Entity is searched
  Entity* getEntityById(std::uint32_t id);

  inline std::string getSceneName()
  {
    return _sceneName;
  }
  inline Camera* getCamera()
  {
    return _camera;
  }

private:
  static Scene _sceneInstance;
  static std::mutex _mutex;

  static Camera* _camera;
  std::vector<Entity> _entities;
  std::vector<Entity> _debugEntities;
  std::string _sceneName = CoreConstants::UNDEFINED_STRING;

  Scene();
  ~Scene();
};
