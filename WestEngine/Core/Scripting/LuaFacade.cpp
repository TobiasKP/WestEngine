#include "LuaFacade.hpp"

#include "../../Constants/LuaAPI.hpp"

#include <algorithm>
#include <cassert>
#include <CoreConstants.hpp>
#include <filesystem>
#include <format>
#include <PathUtils.h>


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
  _logger            = logger;
  std::string luaDir = PathUtils::getExecutableDir() + "/lua/";
  std::ranges::replace(luaDir, '\\', '/');
  std::string luaPath = luaDir + "?.lua;" + luaDir + "?/init.lua";
  luaL_dostring(L, std::format("package.path = '{}' .. ';' .. package.path", luaPath).c_str());
#ifdef DEBUG
  luaL_dostring(L, "DEBUG = true");
#else
  luaL_dostring(L, "DEBUG = false");
#endif
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
      std::format("Lua State error ::: Registering function to stack, function is nullptr or name is empty: {}\n", name));
    return 1;
  }
  lua_pushlightuserdata(L, me);
  lua_pushcclosure(L, f, 1);
  lua_setglobal(L, name.data());
  return 0;
}


bool LuaFacade::onTileClicked(std::int32_t entityId, MouseAction m)
{
  assert(entityId > -1);
  if (m == LMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::WORLD_POS_LCLICK.data());
  }
  else if (m == RMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::WORLD_POS_RCLICK.data());
  }
  if (!lua_isfunction(L, -1))
  {
    _logger->log(Level::Info,
                 std::format("Lua State ::: function for called action: {} is not defined.\n",
                             m == LMOUSE_CLICK ? LuaAPI::WORLD_POS_LCLICK : LuaAPI::WORLD_POS_RCLICK));
    lua_pop(L, 1);
    return 1;
  }
  lua_pushinteger(L, entityId);
  std::int32_t status = lua_pcall(L, 1, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}

bool LuaFacade::onEntityClicked(std::int32_t entityId, MouseAction m)
{
  assert(entityId >= 0);
  if (m == LMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::ENTITY_LCLICK.data());
  }
  else if (m == RMOUSE_CLICK)
  {
    lua_getglobal(L, LuaAPI::ENTITY_RCLICK.data());
  }
  if (!lua_isfunction(L, -1))
  {
    _logger->log(Level::Info,
                 std::format("Lua State ::: function for called action is not defined.\n",
                             m == LMOUSE_CLICK ? LuaAPI::ENTITY_LCLICK : LuaAPI::ENTITY_RCLICK));
    lua_pop(L, 1);
    return 1;
  }

  lua_pushinteger(L, entityId);
  std::int32_t status = lua_pcall(L, 1, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }

  return 0;
}

bool LuaFacade::onEntityCreation(std::int32_t entityId, bool playable)
{
  lua_getglobal(L, LuaAPI::ENTITY_REGISTER.data());
  lua_pushinteger(L, entityId);
  lua_pushboolean(L, playable);
  std::int32_t status = lua_pcall(L, 2, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}

bool LuaFacade::onStateChange(std::int32_t entityId, std::int32_t oldState, std::int32_t newState)
{
  lua_getglobal(L, LuaAPI::STATE_CHANGE.data());
  lua_pushinteger(L, entityId);
  lua_pushinteger(L, oldState);
  lua_pushinteger(L, newState);
  std::int32_t status = lua_pcall(L, 3, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}

bool LuaFacade::onUIRefresh()
{
  lua_getglobal(L, LuaAPI::UI_REFRESH.data());
  std::int32_t status = lua_pcall(L, 0, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }

  return 0;
}

bool LuaFacade::onUIDelete(std::int32_t uiId)
{
  return 0;
}

bool LuaFacade::internalCall(const std::string& toCall, std::int32_t callingId)
{
  lua_getglobal(L, LuaAPI::INTERNAL.data());
  lua_pushstring(L, toCall.data());
  lua_pushinteger(L, callingId);
  std::int32_t status = lua_pcall(L, 2, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return IDLE;
  }
  return 0;
}

LuaFacade::LuaStates LuaFacade::getState(std::int32_t id)
{
  lua_getglobal(L, LuaAPI::GET_STATE.data());
  lua_pushinteger(L, id);
  std::int32_t status = lua_pcall(L, 1, 1, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return IDLE;
  }
  std::int32_t result = lua_tointeger(L, -1);
  lua_remove(L, -1);
  return static_cast<LuaFacade::LuaStates>(result);
}

bool LuaFacade::loadAPI()
{
  std::string filePath = PathUtils::resolve(CoreConstants::LUA_API_FILE);
  if (!std::filesystem::exists(filePath))
  {
    _logger->log(Level::Error, std::format("Lua State error ::: Lua API file: {} - not found!\n", filePath));
    return 1;
  }
  if (luaL_dofile(L, filePath.data()) != 0)
  {
    _logger->log(Level::Error, std::format("Lua State error ::: Loading API file: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  lua_getglobal(L, "Init");
  std::int32_t status = lua_pcall(L, 0, 0, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State error ::: Calling Init: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }
  return 0;
}
