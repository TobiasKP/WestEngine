#include "../CoreHeaders/SceneManager.h"

#include <PathUtils.h>

#include <filesystem>
#include <format>
#include <lua.hpp>

#include "../CoreHeaders/Entity/Camera.h"

SceneManager::SceneManager() : IManager(nullptr)
{
  setName(CoreConstants::SCENE_MANAGER);
  _scene   = nullptr;
  _loader  = nullptr;
  L        = nullptr;
  _builder = nullptr;
}

SceneManager::SceneManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::SCENE_MANAGER);
  _scene   = nullptr;
  _loader  = nullptr;
  L        = nullptr;
  _builder = nullptr;
}

SceneManager::~SceneManager() {}

std::int32_t SceneManager::startup()
{
  _loader     = new ObjectLoader(getLogger());
  _scene      = &Scene::getSceneInstance();
  Camera* cam = new Camera(glm::vec3(0.0, 3.0, 5.0), glm::vec3(25.0f, 0, 0));
  _scene->addCamera(cam);
  _facade = &LuaFacade::getLuaFacadeInstance();
  _facade->startup(getLogger());
  L = _facade->getLuaState();

  _builder = new EntityBuilder(L, _loader);
  assert(_loader != nullptr && _scene != nullptr && L != nullptr && _builder != nullptr);
#ifdef DEBUG
  logDebug(std::format("{} ### instantiated Lua state\n", getName()));
#endif
  return 0;
}

void SceneManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
  _facade->shutdown();
  deleteScene();
}

std::int32_t SceneManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene->init();

  std::string filePath = PathUtils::resolve(CoreConstants::LUA_INIT_FILE.data());
  if (!std::filesystem::exists(filePath))
  {
    logFailure(std::format("{} ### Lua init file: {} - not found! Aborting Scene init ", getName(), filePath));
    return 1;
  }

  luaL_dofile(L, filePath.c_str());
  lua_getglobal(L, "Init");
  lua_call(L, 0, 0);

  // TODO potentially do in update later on
  lua_getglobal(L, "LoadScene");
  lua_pushstring(L, "Intro");
  lua_call(L, 1, 1);
  if (lua_istable(L, -1))
  {
    _builder->createEntities();
  }
  else
  {
    logFailure("SceneManager ### Current Lua Stack does not contain a return table");
  }
  lua_pop(L, 1);
  // This block belongs together for loading all entitys of a given scene

#ifdef DEBUG
  logDebug(std::format("{} ### Scene Initialized\n", getName()));
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### {} init time: {} ms.\n", getName(), getName(), res));
#endif

  return 0;
}

void SceneManager::update()
{
  std::vector<std::uint32_t> removedEntities;
  for (auto& entity : _scene->getEntities())
  {
    if (entity.isDestroyed())
    {
      removedEntities.push_back(entity.getId());
    }
  }
  for (std::uint32_t id : removedEntities)
  {
    Entity* e = _scene->getEntityById(id);
    removeEntityFromScene(*e);
  }
}

void SceneManager::removeEntityFromScene(const Entity& entity)
{
#ifdef DEBUG
  logDebug(std::format("{} ### Removing Entitiy from Scene: {}\n", getName(), entity.getId()));
#endif
  _scene->removeEntity(entity);
  if (!entity.isDebugEntity())
  {
    _loader->unloadModel((Model*)entity.getComponent(BitMasks::Components::MODEL),
                         (Material*)entity.getComponent(BitMasks::Components::MATERIAL));
  }
}

void SceneManager::deleteScene()
{
  _loader->cleanup();
#ifdef DEBUG
  logDebug(std::format("{} ### Cleaned up GPU memory\n", getName()));
#endif
  _scene->deleteScene();
#ifdef DEBUG
  logDebug(std::format("{} ### Deleted Scene\n", getName()));
#endif
}
