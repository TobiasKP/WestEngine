#pragma once

#include <cstdint>
#include <glm/glm.hpp>
#include <lua.hpp>
#include <string>
#include <WestLogger.h>

class LuaFacade
{
public:
  enum MouseAction { LMOUSE_CLICK, RMOUSE_CLICK, MMOUSE_CLICK };
  enum LuaStates { IDLE, MOVING, INSPECTING, ACTION };

  static LuaFacade& getLuaFacadeInstance();
  void shutdown();
  void startup(WestLogger* logger);
  bool registerCFunction(int (*f)(lua_State*), std::string name, void* me);

  //
  bool onEntityCreation(std::int32_t entityId, bool playable);
  bool onTileClicked(std::int32_t entityId, MouseAction m);
  bool onEntityClicked(std::int32_t entityId, MouseAction m);
  bool onStateChange(std::int32_t entityId, std::int32_t oldState, std::int32_t newState);
  bool onUIRefresh();
  LuaStates getState(std::int32_t calleeId);

  inline lua_State* getLuaState()
  {
    return L;
  }


private:
  LuaFacade();
  ~LuaFacade();

  bool loadAPI();

  WestLogger* _logger;
  lua_State* L;
};
