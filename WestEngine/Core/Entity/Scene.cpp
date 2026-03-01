#include "../../CoreHeaders/Entity/Scene.h"

#include "../../CoreHeaders/Components/Shader.h"

#include <algorithm>


std::mutex Scene::_mutex;
Camera* Scene::_camera;
World* Scene::_world;

Scene& Scene::getSceneInstance()
{
  std::lock_guard<std::mutex> lock(_mutex);

  static Scene instance;
  return instance;
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
  _entities.reserve(CoreConstants::MAX_ENTITY_SIZE);
#ifdef DEBUG
  _debugEntities.reserve(256);
#endif
}

const std::vector<Entity>& Scene::getEntities() const
{
  return _entities;
}

#ifdef DEBUG
const std::vector<Entity>& Scene::getDebugEntities() const
{
  return _debugEntities;
}
#endif

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

void Scene::addEntity(Entity&& entity)
{
  if (_entities.size() >= CoreConstants::MAX_ENTITY_SIZE)
  {
    WestLogger::getLoggerInstance().log(
      Level::Error,
      std::format("Could not add Entity to Scene, max number {} of entites reached", CoreConstants::MAX_ENTITY_SIZE));
    return;
  }
  std::lock_guard<std::mutex> lock(_mutex);
  insertEntityByGroup(std::move(entity));
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

void Scene::addRegistry(ComponentRegistry* reg)
{
  assert(reg != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _registry = std::move(reg);
}

void Scene::insertEntityByGroup(Entity&& entity)
{
  GLint group = _registry->getComponent<Shader>(entity.getId())->shadergroup;

  auto pos = std::upper_bound(_entities.begin(),
                              _entities.end(),
                              group,
                              [&](GLint g, const Entity& e)
                              { return g < _registry->getComponent<Shader>(e.getId())->shadergroup; });
  _entities.insert(pos, std::move(entity));
}

void Scene::removeEntity(const Entity& entity)
{
  std::lock_guard<std::mutex> lock(_mutex);

  auto entityIt =
    std::find_if(_entities.begin(), _entities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

  if (entityIt != _entities.end())
  {
    _entities.erase(entityIt);
  }

#ifdef DEBUG
  if (entity.isDebugEntity())
  {
    auto debugIt = std::find_if(
      _debugEntities.begin(), _debugEntities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

    if (debugIt != _debugEntities.end())
    {
      _debugEntities.erase(debugIt);
    }
  }
#endif
}
