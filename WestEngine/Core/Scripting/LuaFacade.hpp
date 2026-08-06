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
  enum LuaStates { IDLE, MOVING, INSPECTING, ACTION, ATTACKING };

  static LuaFacade& getLuaFacadeInstance();
  void shutdown();
  void startup(WestLogger* logger);
  bool registerCFunction(int (*f)(lua_State*), std::string name, void* me);

  //
  bool onEntityCreation(std::int32_t entityId, bool playable, std::int32_t health);
  bool onEntityDestroy(std::int32_t entityId);
  bool onTileClicked(std::int32_t entityId, MouseAction m);
  bool onEntityClicked(std::int32_t entityId, MouseAction m);
  bool onStateChange(std::int32_t entityId, std::int32_t oldState, std::int32_t newState);
  bool onUIRefresh();
  bool onUIDelete(std::int32_t uiId);
  bool onAttack(std::uint32_t attackerId,
                std::uint32_t bulletType,
                std::uint32_t wRange,
                std::uint32_t distanceToTarget,
                std::uint32_t wDamage,
                float wAccuracy,
                std::uint32_t targetId,
                float spawnX,
                float spawnY);
  bool entityQueue();
  bool internalCall(const std::string& toCall, std::int32_t callingId);
  bool aiCall(std::int32_t entityId);
  bool levelEnd();
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

  WestLogger* _logger;
  lua_State* L;
};
