#pragma once

#include "../Components/ComponentRegistry.hpp"
#include "Camera.h"
#include "Entity.h"
#include "World.hpp"

#include <CoreConstants.hpp>
#include <mutex>
#include <vector>

class Scene
{
public:
  Scene();
  ~Scene();

  // Functions
  void init();
  void addEntity(Entity&& entity);
  void addDebugEntity(Entity&& entity);
  void addWorld(std::shared_ptr<World> world);
  void addCamera(std::shared_ptr<Camera> cam);
  void addRegistry(std::shared_ptr<ComponentRegistry> reg);
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

  inline std::shared_ptr<World> getWorld()
  {
    return _world;
  }
  inline std::shared_ptr<ComponentRegistry> getRegistry()
  {
    return _registry;
  }
  inline std::shared_ptr<Camera> getCamera()
  {
    return _camera;
  }

private:
  static std::mutex _mutex;

  std::shared_ptr<ComponentRegistry> _registry;
  std::shared_ptr<World> _world;
  std::shared_ptr<Camera> _camera;
  std::vector<Entity> _entities;
  std::vector<Entity> _debugEntities;
  std::string _sceneName = CoreConstants::UNDEFINED_STRING.data();

  void insertEntityByGroup(Entity&& entity);
};
