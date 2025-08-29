#include "../CoreHeaders/SceneManager.h"

#include <TimeUtils.hpp>
#include <lua.hpp>

#ifdef _WIN32
<include> direct.h
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif

#include "../CoreHeaders/Entity/Camera.h"

SceneManager::SceneManager()
    : IManager(nullptr) {
  setName(CoreConstants::SCENE_MANAGER);
}

SceneManager::SceneManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::SCENE_MANAGER);
}

SceneManager::~SceneManager() {}

std::int32_t SceneManager::startup() {
  _loader = new ObjectLoader(getLogger());
  _scene = &Scene::getSceneInstance();
  Camera *cam = new Camera(glm::vec3(0.0, 0.0, 5.0), glm::vec3(0));
  _scene->addCamera(cam);
  L = luaL_newstate();
  luaL_openlibs(L);
  _builder = new EntityBuilder(L, _loader);
#ifdef DEBUG
  logDebug(std::format("{} ### instantiated lus state\n", getName()));
#endif
  return 0;
}

void SceneManager::shutdown() {
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
  lua_close(L);
  deleteScene();
}

std::int32_t SceneManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene->init();

  char cwd[PATH_MAX];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return 1;

  snprintf(filePath, sizeof(filePath), "%s%s", cwd,
           CoreConstants::LUA_INIT_FILE.c_str());
  luaL_dofile(L, filePath);
  lua_getglobal(L, "Init");
  lua_call(L, 0, 0);

  // TODO potentially do in update later on
  lua_getglobal(L, "LoadScene");
  lua_pushstring(L, "Intro");
  lua_call(L, 1, 1);
  if (lua_istable(L, -1))
    _builder->createEntities();
  else
    logFailure(
        "SceneManager ### Current Lua Stack does not contain a return table");
  lua_pop(L, 1);
  // This block belongs together for loading all entitys of a given scene

#ifdef DEBUG
  logDebug(std::format("{} ### Scene Initialized\n", getName()));
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(
      std::format("{} ### %s init time: {} ms.\n", getName(), getName(), res));
#endif

  return 0;
}

void SceneManager::update() {
  for (auto &entity : _scene->getEntities()) {
    if (entity->isDestroyed())
      removeEntityFromScene(entity);
  }
}

void SceneManager::removeEntityFromScene(Entity *entity) {
#ifdef DEBUG
  logDebug(std::format("{} ### Removing Entitiy from Scene: {}\n", getName(),
                       entity->getId()));
#endif
  _scene->removeEntity(entity);
  if (!entity->isDebugEntity())
    _loader->unloadModel(
        (Model *)entity->getComponent(BitMasks::Components::MODEL));
}

void SceneManager::deleteScene() {
  _loader->cleanup();
#ifdef DEBUG
  logDebug(std::format("{} ### Cleaned up GPU memory\n", getName()));
#endif
  _scene->deleteScene();
#ifdef DEBUG
  logDebug(std::format("{} ### Deleted Scene\n", getName()));
#endif
}
