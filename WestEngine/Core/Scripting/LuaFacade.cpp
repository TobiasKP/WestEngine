#include "LuaFacade.hpp"

#include "../../Constants/LuaAPI.hpp"

#include <cassert>
#include <CoreConstants.hpp>
#include <filesystem>


#ifdef _WIN32
#include <direct.h>
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif


LuaFacade& LuaFacade::getLuaFacadeInstance()
{
  static LuaFacade instance;
  return instance;
}

LuaFacade::LuaFacade()
{
  _logger = nullptr;
  L       = nullptr;
}

LuaFacade::~LuaFacade() {}

void LuaFacade::shutdown()
{
  lua_close(L);
}
void LuaFacade::startup(WestLogger* logger)
{
  L = luaL_newstate();
  luaL_openlibs(L);
  _logger = logger;
  if (loadAPI() != 0)
  {
    _logger->log(Level::Error, std::format("Lua State error ::: Lua API file"));
  }
}


bool LuaFacade::registerCFunction(int (*f)(lua_State*), std::string name, void* me)
{
  if (f == nullptr || name.empty())
  {
    _logger->log(
      Level::Error,
      std::format("Lua State error ::: Registering function to stack, function is nullptr or name is empty: {}", name));
    return 1;
  }
  lua_pushlightuserdata(L, me);
  lua_pushcclosure(L, f, 1);
  lua_setglobal(L, name.c_str());
  return 0;
}


bool LuaFacade::onTileClicked(std::int32_t calleeId, MouseAction m, glm::vec3 destination)
{
  assert(calleeId > -1);
  if (m == LMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::WORLD_POS_LCLICK.c_str());
  }
  else if (m == RMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::WORLD_POS_RCLICK.c_str());
  }
  if (!lua_isfunction(L, -1))
  {
    _logger->log(Level::Info, "Lua State ::: function for called action is not defined.\n");
    lua_pop(L, 1);
    return 1;
  }
  lua_pushinteger(L, calleeId);
  lua_pushnumber(L, destination.x);
  lua_pushnumber(L, destination.y);
  lua_pushnumber(L, destination.z);
  std::int32_t status = lua_pcall(L, 4, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}

bool LuaFacade::onEntityClicked(std::int32_t calleeId, MouseAction m, std::int32_t entity_id)
{
  assert(calleeId > 0 && entity_id >= 0);
  return 0;
}

bool LuaFacade::onStateChange(std::int32_t calleeId, std::int32_t state)
{
  lua_getglobal(L, LuaAPI::STATE_CHANGE.c_str());
  lua_pushinteger(L, calleeId);
  lua_pushinteger(L, state);
  std::int32_t status = lua_pcall(L, 2, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}

bool LuaFacade::loadAPI()
{
  char cwd[PATH_MAX];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
  {
    return 1;
  }

  snprintf(filePath, sizeof(filePath), "%s%s", cwd, CoreConstants::LUA_API_FILE);
  if (!std::filesystem::exists(filePath))
  {
    _logger->log(Level::Error, std::format("Lua State error ::: Lua API file: {} - not found!", filePath));
    return 1;
  }
  luaL_dofile(L, filePath);
  lua_getglobal(L, "Init");
  lua_call(L, 0, 0);
  return 0;
}
