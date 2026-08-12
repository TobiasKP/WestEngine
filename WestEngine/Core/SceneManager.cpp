#include "../CoreHeaders/SceneManager.h"

#include "../Constants/LuaAPI.hpp"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <filesystem>
#include <format>
#include <lua.hpp>
#include <PathUtils.h>
#include <WestRendererFacade.hpp>

SceneManager::SceneManager() : IManager(nullptr)
{
  setName(CoreConstants::SCENE_MANAGER);
  _scene    = nullptr;
  _ebuilder = nullptr;
  _wbuilder = nullptr;
  _registry = nullptr;
  L         = nullptr;
}

SceneManager::SceneManager(WestLogger* logger,
                           const std::shared_ptr<EventDispatcher>& d,
                           const std::shared_ptr<Scene>& s)
  : IManager(logger)
{
  setName(CoreConstants::SCENE_MANAGER);
  _dispatcher = d;
  _scene      = s;
  _ebuilder   = nullptr;
  _wbuilder   = nullptr;
  _registry   = nullptr;
  L           = nullptr;
}

SceneManager::~SceneManager() {}

std::int32_t SceneManager::startup()
{
  _dataFacade               = &WestData::WestAssetFacade::getAssetFacade();
  std::shared_ptr<Camera> c = std::make_shared<Camera>(glm::vec3(0.0, 3.0, 5.0), glm::vec3(25.0f, 0, 0));
  _registry                 = std::make_shared<ComponentRegistry>();

  _scene->addRegistry(_registry);
  _scene->addCamera(std::move(c));

  _facade = &LuaFacade::getLuaFacadeInstance();
  _facade->startup(getLogger());
  L = _facade->getLuaState();

  _ebuilder = std::make_unique<EntityBuilder>(L, _registry, _scene);
  _wbuilder = std::make_unique<WorldBuilder>(L, _registry, _scene);
  assert(_scene != nullptr && _wbuilder != nullptr && _ebuilder != nullptr);
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
  deleteScene();
  _facade->shutdown();
}

std::int32_t SceneManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  _registry->registerComponent<Movement>();
  _registry->registerComponent<Position>();
  _registry->registerComponent<AABB>();
  _registry->registerComponent<Control>();
  _registry->registerComponent<Health>();
  _registry->registerComponent<Projectile>();
  _registry->registerComponent<Equipment>();
  _registry->registerComponent<Appearance>();
  _facade->registerCFunction(getHealth, LuaAPI::C_GETHEALTH.data(), this);
  _facade->registerCFunction(getPosition, LuaAPI::C_GETPOSITION.data(), this);
  _scene->init();

  std::string filePath = PathUtils::resolve(CoreConstants::LUA_INIT_FILE.data());
  if (!std::filesystem::exists(filePath))
  {
    logFailure(std::format("{} ### Lua init file: {} - not found! Aborting Scene init ", getName(), filePath));
    return 1;
  }
  luaL_dofile(L, filePath.c_str());
  lua_getglobal(L, "Init");
  std::int32_t result = lua_pcall(L, 0, 0, 0);
  if (result != 0)
  {
    logFailure(
      std::format("{} ### Lua init file: {} - Init function not found, Aborting Scene init ", getName(), filePath));
    return 1;
  }

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
  bool result  = _facade->emit(EventIdentifiers::LEVEL_END, EmptyPayload{});
  result      |= _facade->emit(EventIdentifiers::ENTITY_QUEUE, EmptyPayload{});
  if (result == 1)
  {
    logFailure(std::format("{} ### failure calling lua frame game updates.\n", getName()));
    return;
  }
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
  logDebug(std::format("{} ### Removing Entity from Scene: {}\n", getName(), entity.getId()));
#endif

  if (!entity.isDebugEntity())
  {
    const std::string& guid = entity.getModelGuid();
    bool stillInUse         = false;
    for (const Entity& e : _scene->getEntities())
    {
      if (e.getId() != entity.getId() && e.getModelGuid() == guid)
      {
        stillInUse = true;
        break;
      }
    }
    if (!stillInUse)
    {
      WestRenderer::WestRendererFacade::getRendererFacade().cleanupModel(guid);
      _dataFacade->deleteModelFromScene(guid);
    }
    LuaFacade::getLuaFacadeInstance().emit(EventIdentifiers::ENTITY_DESTROYED,
                                           EntityPayload{.entityId = entity.getId()});
  }
  _registry->removeAllComponents(entity.getId());
  _scene->removeEntity(entity);
}

void SceneManager::deleteScene()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Cleaned up GPU memory\n", getName()));
#endif
  std::vector<std::uint32_t> removedEntities;
  for (auto& entity : _scene->getEntities())
  {
    removedEntities.push_back(entity.getId());
  }
  for (std::uint32_t id : removedEntities)
  {
    Entity* e = _scene->getEntityById(id);
    removeEntityFromScene(*e);
  }

  _scene->deleteScene();
#ifdef DEBUG
  logDebug(std::format("{} ### Deleted Scene\n", getName()));
#endif
}

int SceneManager::getHealth(lua_State* L)
{
  SceneManager* me = (SceneManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint32_t id = lua_tointeger(L, 1);
  assert(me->_registry->getComponent<Health>(id) != nullptr);
  std::uint16_t health = me->_registry->getComponent<Health>(id)->current;

  lua_pushinteger(L, health);
  return 1;
}

int SceneManager::getPosition(lua_State* L)
{
  SceneManager* me = (SceneManager*)lua_touserdata(L, lua_upvalueindex(1));
  std::uint32_t id = lua_tointeger(L, 1);
  assert(me->_registry->getComponent<Position>(id) != nullptr);
  glm::vec3 pos    = me->_registry->getComponent<Position>(id)->position;
  glm::vec2 screen = PositionCalculation::getScreenPosition(pos, me->_scene->getCamera());
  lua_pushnumber(L, screen.x);
  lua_pushnumber(L, screen.y);
  return 2;
}
