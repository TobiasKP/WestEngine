#include "../../CoreHeaders/Entity/Scene.h"

#include <algorithm>

Scene Scene::_sceneInstance;
std::mutex Scene::_mutex;
Camera* Scene::_camera;
World* Scene::_world;

Scene& Scene::getSceneInstance()
{
  std::lock_guard<std::mutex> lock(_mutex);

  static Scene instance;
  return _sceneInstance;
}

Scene::Scene()
{
  _camera = nullptr;
  _world  = nullptr;
}

Scene::~Scene() {}

void Scene::deleteScene()
{
  delete _world;
  delete _camera;
  _entities.clear();
  _world  = nullptr;
  _camera = nullptr;
}

void Scene::init()
{
  std::lock_guard<std::mutex> lock(_mutex);
  _entities.reserve(512);
#ifdef DEBUG
  _debugEntities.reserve(256);
#endif
}

// TODO: full vector copy per frame, debug entities accessed outside lock (race condition)
std::vector<Entity> Scene::getEntities()
{
  std::vector<Entity> entities;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    entities = _entities;
  }
#ifdef DEBUG
  if (_debugEntities.size() > 0)
  {
    entities.insert(entities.end(), _debugEntities.begin(), _debugEntities.end());
  }
#endif
  return entities;
}

Entity* Scene::getEntityById(std::uint32_t id)
{
  std::lock_guard<std::mutex> lock(_mutex);
  auto entityIt = std::find_if(_entities.begin(), _entities.end(), [id](const Entity& en) { return en.getId() == id; });
  if (entityIt != _entities.end())
  {
    return &(*entityIt);
  }

#ifdef DEBUG
  auto debugEntityIt =
    std::find_if(_debugEntities.begin(), _debugEntities.end(), [id](const Entity& en) { return en.getId() == id; });
  if (debugEntityIt != _debugEntities.end())
  {
    return &(*debugEntityIt);
  }
#endif

  return nullptr;
}

// TODO Sort by entity shader group
void Scene::addEntity(Entity&& entity)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _entities.emplace_back(std::move(entity));
}

void Scene::addDebugEntity(Entity&& entity)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _debugEntities.emplace_back(std::move(entity));
}

void Scene::addCamera(Camera* cam)
{
  assert(cam != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _camera = cam;
}

void Scene::addWorld(World* world)
{
  assert(world != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _world = std::move(world);
}

void Scene::removeEntity(const Entity& entity)
{
  auto entityIt =
    std::find_if(_entities.begin(), _entities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

  if (entityIt != _entities.end())
  {
    std::lock_guard<std::mutex> lock(_mutex);
    _entities.erase(entityIt);
  }

#ifdef DEBUG
  if (entity.isDebugEntity())
  {
    auto entityIt = std::find_if(
      _debugEntities.begin(), _debugEntities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

    if (entityIt != _debugEntities.end())
    {
      std::lock_guard<std::mutex> lock(_mutex);
      _debugEntities.erase(entityIt);
    }
  }
#endif
}
