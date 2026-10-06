#include "LuaFacade.hpp"

#include "../../Constants/LuaAPI.hpp"

#include <algorithm>
#include <cassert>
#include <CoreConstants.hpp>
#include <filesystem>
#include <format>
#include <PathUtils.h>


static int appendTraceback(lua_State* L)
{
  const char* message = lua_tostring(L, -1);
  luaL_traceback(L, L, message != nullptr ? message : "unknown lua error", 1);
  return 1;
}


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
  exportEventIdentifiers();
  // logging has to be available before loadAPI, Init may already log
  exportLogLevels();
  registerCFunction(westLog, LuaAPI::C_WEST_LOG.data(), this);
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
      std::format("Lua State error ::: Registering function to stack, function is nullptr or name is empty: {}\n",
                  name));
    return 1;
  }
  lua_pushlightuserdata(L, me);
  lua_pushcclosure(L, f, 1);
  lua_setglobal(L, name.data());
  return 0;
}


bool LuaFacade::emit(const EventIdentifiers event, const EventPayload& payload)
{
  const std::int32_t base = lua_gettop(L);
  lua_pushcfunction(L, appendTraceback);
  lua_getglobal(L, LuaAPI::ON_EVENT.data());
  if (!lua_isfunction(L, -1))
  {
    _logger->log(Level::Error,
                 std::format("Lua State ::: {} is not defined, event {} can not be delivered.\n",
                             LuaAPI::ON_EVENT,
                             eventName(event)));
    lua_settop(L, base);
    return 1;
  }
  lua_pushinteger(L, static_cast<lua_Integer>(event));
  std::visit([this](const auto& p) { p.push(L); }, payload);
  if (lua_pcall(L, 2, 0, base + 1) != LUA_OK)
  {
    _logger->log(Level::Error,
                 std::format("Lua State ::: Error handling event {}: {}\n", eventName(event), lua_tostring(L, -1)));
    lua_settop(L, base);
    return 1;
  }
  lua_settop(L, base);
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


std::int32_t LuaFacade::getActionPoints(std::int32_t id)
{
  lua_getglobal(L, LuaAPI::GET_ACTION_POINTS.data());
  lua_pushinteger(L, id);
  std::int32_t status = lua_pcall(L, 1, 1, 0);
  if (status != 0)
  {
    _logger->log(Level::Error, std::format("Lua State ::: Error executing function: {}\n", lua_tostring(L, -1)));
    lua_pop(L, 1);
    return 1;
  }

  std::int32_t result = lua_tointeger(L, -1);
  lua_remove(L, -1);
  return result;
}

void LuaFacade::exportEventIdentifiers()
{
  lua_createtable(L, 0, static_cast<std::int32_t>(EVENT_COUNT));
  for (std::size_t i = 0; i < EVENT_COUNT; i++)
  {
    lua_pushinteger(L, static_cast<lua_Integer>(i));
    lua_setfield(L, -2, eventName(static_cast<EventIdentifiers>(i)).data());
  }
  lua_setglobal(L, LuaAPI::C_EVENTS.data());
}

void LuaFacade::exportLogLevels()
{
  lua_createtable(L, 0, 3);
  lua_pushinteger(L, static_cast<lua_Integer>(Level::Info));
  lua_setfield(L, -2, "Info");
  lua_pushinteger(L, static_cast<lua_Integer>(Level::Error));
  lua_setfield(L, -2, "Error");
  lua_pushinteger(L, static_cast<lua_Integer>(Level::Cycle));
  lua_setfield(L, -2, "Cycle");
  lua_setglobal(L, LuaAPI::C_LOG_LEVELS.data());
}

int LuaFacade::westLog(lua_State* L)
{
  LuaFacade* me           = (LuaFacade*)lua_touserdata(L, lua_upvalueindex(1));
  assert(me != nullptr);
  const lua_Integer level = luaL_checkinteger(L, 1);
  const char* message     = luaL_checkstring(L, 2);
  if (level < static_cast<lua_Integer>(Level::Info) || level > static_cast<lua_Integer>(Level::Cycle))
  {
    return luaL_argerror(L, 1, "invalid log level, use a LogLevel value");
  }
  me->_logger->log(static_cast<Level>(level), std::format("{}\n", message));
  return 0;
}

bool LuaFacade::loadAPI()
{
  std::string filePath = PathUtils::resolve(std::string(CoreConstants::LUA_API_FILE));
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
