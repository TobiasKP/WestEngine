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
  enum LuaStates { IDLE, MOVING, MOVING_FINISHED, INSPECTING, ACTION };

  static LuaFacade& getLuaFacadeInstance();
  void shutdown();
  void startup(WestLogger* logger);
  bool registerCFunction(int (*f)(lua_State*), std::string name, void* me);

  //
  bool onTileClicked(std::int32_t calleeId, MouseAction m, glm::vec3 destination);
  bool onEntityClicked(std::int32_t calleeId, MouseAction m, std::int32_t entity_id);
  bool onStateChange(std::int32_t calleeId, std::int32_t state);
  bool onUIRefresh(std::int32_t calleeId);
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
