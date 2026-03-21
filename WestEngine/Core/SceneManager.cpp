#include "../CoreHeaders/SceneManager.h"

#include "../Constants/LuaAPI.hpp"

#include <filesystem>
#include <format>
#include <lua.hpp>
#include <PathUtils.h>

SceneManager::SceneManager() : IManager(nullptr)
{
  setName(CoreConstants::SCENE_MANAGER);
  _scene    = nullptr;
  _loader   = nullptr;
  _ebuilder = nullptr;
  _wbuilder = nullptr;
  _registry = nullptr;
  L         = nullptr;
}

SceneManager::SceneManager(WestLogger* logger, const std::shared_ptr<EventDispatcher>& d, const std::shared_ptr<Scene>& s)
  : IManager(logger)
{
  setName(CoreConstants::SCENE_MANAGER);
  _dispatcher = d;
  _scene      = s;
  _loader     = nullptr;
  _ebuilder   = nullptr;
  _wbuilder   = nullptr;
  _registry   = nullptr;
  L           = nullptr;
}

SceneManager::~SceneManager() {}

std::int32_t SceneManager::startup()
{
  std::shared_ptr<Camera> c = std::make_shared<Camera>(glm::vec3(0.0, 3.0, 5.0), glm::vec3(25.0f, 0, 0));
  _registry                 = std::make_shared<ComponentRegistry>();
  _loader                   = new ObjectLoader(getLogger());

  _scene->addRegistry(_registry);
  _scene->addCamera(std::move(c));

  _facade = &LuaFacade::getLuaFacadeInstance();
  _facade->startup(getLogger());
  L = _facade->getLuaState();

  _ebuilder = std::make_unique<EntityBuilder>(L, _loader, _registry, _scene);
  _wbuilder = std::make_unique<WorldBuilder>(L, _loader, _registry, _scene);
  assert(_loader != nullptr && _scene != nullptr && _wbuilder != nullptr && _ebuilder != nullptr);
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
  _ebuilder.reset();
  _wbuilder.reset();
  _facade->shutdown();
  deleteScene();
}

std::int32_t SceneManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  _registry->registerComponent<Model>();
  _registry->registerComponent<Material>();
  _registry->registerComponent<Shader>();
  _registry->registerComponent<Movement>();
  _registry->registerComponent<Position>();
  _registry->registerComponent<AABB>();
  _registry->registerComponent<Control>();
  _registry->registerComponent<Health>();
  _facade->registerCFunction(getHealth, LuaAPI::C_GETHEALTH.data(), this);
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
    Model* mo    = _registry->getComponent<Model>(entity.getId());
    Material* ma = _registry->getComponent<Material>(entity.getId());
    if (!_registry->removeComponent<Material>(entity.getId()))
    {
      logFailure(std::format("{} ### error deleting Material Component from entity: {}, might cause memory leaks "
                             "program will continue running",
                             getName(),
                             entity.getId()));
    }

    if (!_registry->removeComponent<Model>(entity.getId()))
    {
      logFailure(std::format(
        "{} ### error deleting Model Component from entity: {}, might cause memory leaks program will continue running",
        getName(),
        entity.getId()));
    }
    _loader->unloadModel(mo, ma);
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

int SceneManager::getHealth(lua_State* L)
{
  SceneManager* me     = (SceneManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint32_t id     = lua_tointeger(L, 1);
  std::uint16_t health = me->_registry->getComponent<Health>(id)->current;
  lua_pushinteger(L, health);
  return 1;
}
