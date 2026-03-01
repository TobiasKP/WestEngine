#pragma once

#include "Camera.h"
#include "Entity.h"
#include "World.hpp"
#include "../Components/ComponentRegistry.hpp"

#include <CoreConstants.hpp>
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
  void addWorld(World* world);
  void addRegistry(ComponentRegistry* reg);
  void removeEntity(const Entity& entity);
  void deleteScene();

  // Getter
  const std::vector<Entity>& getEntities() const;
#ifdef DEBUG
  const std::vector<Entity>& getDebugEntities() const;
#endif
  // TODO: register ID in hashmap saving index in vector to have faster access if a specific Entity is searched
  Entity* getEntityById(std::uint32_t id);

  inline std::string getSceneName()
  {
    return _sceneName;
  }
  inline Camera* getCamera()
  {
    return _camera;
  }
  inline World* getWorld()
  {
    return _world;
  }
  inline ComponentRegistry* getRegistry()
  {
    return _registry;
  }

private:
  static std::mutex _mutex;

  static Camera* _camera;
  static World* _world;
  ComponentRegistry* _registry;
  std::vector<Entity> _entities;
  std::vector<Entity> _debugEntities;
  std::string _sceneName = CoreConstants::UNDEFINED_STRING.data();

  void insertEntityByGroup(Entity&& entity);

  Scene();
  ~Scene();
};
