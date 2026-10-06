#pragma once

#include "../../Constants/InternalEvents.hpp"
#include "../Events/EventPayload.hpp"

#include <cstdint>
#include <glm/glm.hpp>
#include <lua.hpp>
#include <string>
#include <WestLogger.h>

class LuaFacade
{
public:
  enum LuaStates { IDLE, MOVING, INSPECTING, ACTION, ATTACKING };

  static LuaFacade& getLuaFacadeInstance();
  void shutdown();
  void startup(WestLogger* logger);
  bool registerCFunction(int (*f)(lua_State*), std::string name, void* me);

  bool emit(const EventIdentifiers event, const EventPayload& payload);

  LuaStates getState(std::int32_t calleeId);
  std::int32_t getActionPoints(std::int32_t id);


  inline lua_State* getLuaState()
  {
    return L;
  }


private:
  LuaFacade();
  ~LuaFacade();

  bool loadAPI();
  void exportEventIdentifiers();
  void exportLogLevels();

  static int westLog(lua_State* L);

  WestLogger* _logger;
  lua_State* L;
};
