#include "../../CoreHeaders/Entity/Scene.h"

#include <algorithm>
#include <format>

std::mutex Scene::_mutex;

Scene::Scene() {}

Scene::~Scene() {}

void Scene::deleteScene()
{
  _world.reset();
  _camera.reset();
  _registry.reset();
  for (Entity& e : _entities)
  {
    Config::freedEntityIds.push(e.getId());
  }
  _entities.clear();
}

void Scene::init()
{
  std::lock_guard<std::mutex> lock(_mutex);
  _entities.reserve(CoreConstants::MAX_ENTITY_SIZE);
#ifdef DEBUG
  _debugEntities.reserve(256);
#endif
}

std::vector<Entity>& Scene::getEntities()
{
  return _entities;
}

#ifdef DEBUG
std::vector<Entity>& Scene::getDebugEntities()
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
  WestLogger::getLoggerInstance().log(Level::Error, std::format("Can not receive Entity for id: {} from Scene", id));
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

void Scene::addWorld(std::shared_ptr<World> w)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _world = w;
}


void Scene::addCamera(std::shared_ptr<Camera> c)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _camera = c;
}

void Scene::addRegistry(std::shared_ptr<ComponentRegistry> r)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _registry = r;
}

void Scene::insertEntityByGroup(Entity&& entity)
{
  GLint group = entity.getShaderId();

  auto pos = std::upper_bound(
    _entities.begin(), _entities.end(), group, [&](GLint g, const Entity& e) { return g < e.getShaderId(); });
  _entities.insert(pos, std::move(entity));
}

void Scene::removeEntity(const Entity& entity)
{
  std::lock_guard<std::mutex> lock(_mutex);

  auto entityIt =
    std::find_if(_entities.begin(), _entities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

  if (entityIt != _entities.end())
  {
    _world->removeEntityFromGrid(entity.getId());
    Config::freedEntityIds.push(entityIt->getId());
    _entities.erase(entityIt);
  }

#ifdef DEBUG
  if (entity.isDebugEntity())
  {
    auto debugIt = std::find_if(
      _debugEntities.begin(), _debugEntities.end(), [&](const Entity& en) { return en.getId() == entity.getId(); });

    if (debugIt != _debugEntities.end())
    {
      Config::freedEntityIds.push(debugIt->getId());
      _debugEntities.erase(debugIt);
    }
  }
#endif
}

void Scene::removeEntity(std::uint32_t id)
{
  const Entity* e = getEntityById(id);
  removeEntity(*e);
}
