#include "../../CoreHeaders/Entity/Scene.h"

#include <algorithm>

Scene Scene::_sceneInstance;
std::mutex Scene::_mutex;
Camera *Scene::_camera;

Scene &Scene::getSceneInstance() {
  std::lock_guard<std::mutex> lock(_mutex);

  static Scene instance;
  return _sceneInstance;
}

Scene::Scene() {}

Scene::~Scene() { }

void Scene::deleteScene() {
  delete _camera;
  _entities.clear();
}

void Scene::init() {
  std::lock_guard<std::mutex> lock(_mutex);
  _entities.reserve(512);
#ifdef DEBUG
  _debugEntities.reserve(256);
#endif
}

std::vector<Entity *> Scene::getEntities() {
  std::vector<Entity *> entities;
  {
    std::lock_guard<std::mutex> lock(_mutex);
    entities = _entities;
  }
#ifdef DEBUG
  if (_debugEntities.size() > 0)
    entities.insert(entities.end(), _debugEntities.begin(),
                    _debugEntities.end());
#endif
  return entities;
}

void Scene::addEntity(Entity *entity) {
  assert(entity != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _entities.emplace_back(entity);
}

void Scene::addDebugEntity(Entity *entity) {
  assert(entity != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _debugEntities.emplace_back(entity);
}

void Scene::addCamera(Camera *cam) {
  assert(cam != nullptr);
  std::lock_guard<std::mutex> lock(_mutex);
  _camera = cam;
}

void Scene::removeEntity(Entity *entity) {
  auto entityIt =
      std::find_if(_entities.begin(), _entities.end(),
                   [&](Entity *en) { return en->getId() == entity->getId(); });

  if (entityIt != _entities.end()) {
    std::lock_guard<std::mutex> lock(_mutex);
    _entities.erase(entityIt);
  }

#ifdef DEBUG
  if (entity->isDebugEntity()) {
    auto entityIt = std::find_if(
        _debugEntities.begin(), _debugEntities.end(),
        [&](Entity *en) { return en->getId() == entity->getId(); });

    if (entityIt != _debugEntities.end()) {
      std::lock_guard<std::mutex> lock(_mutex);
      _debugEntities.erase(entityIt);
    }
  }
#endif
}
