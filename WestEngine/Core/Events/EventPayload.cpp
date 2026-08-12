#include "EventPayload.hpp"

#include "../Scripting/LuaTable.hpp"

void MousePayload::push(lua_State* L) const
{
  LuaTable(L, 3).set("x", x).set("y", y).set("scroll", scroll);
}

void KeyboardPayload::push(lua_State* L) const
{
  LuaTable(L, 2).set("key", key).set("action", action);
}

void GamePayload::push(lua_State* L) const
{
  LuaTable(L, 1).set("turn", turn);
}

void AttackPayload::push(lua_State* L) const
{
  LuaTable(L, 2).set("attacker", attacker).set("target", target);
}

void InterfacePayload::push(lua_State* L) const
{
  LuaTable(L, 3).set("event", event).set("entityId", entityId).set("newValue", newValue);
}

void ActionFinishedPayload::push(lua_State* L) const
{
  LuaTable(L, 1).set("entityId", entityId);
}

void EntityPayload::push(lua_State* L) const
{
  LuaTable(L, 1).set("entityId", entityId);
}

void EntityCreatedPayload::push(lua_State* L) const
{
  LuaTable(L, 3).set("entityId", entityId).set("playable", playable).set("health", health);
}

void StateChangePayload::push(lua_State* L) const
{
  LuaTable(L, 3).set("entityId", entityId).set("oldState", oldState).set("newState", newState);
}

void EntityAttackPayload::push(lua_State* L) const
{
  LuaTable(L, 9)
    .set("attacker", attacker)
    .set("target", target)
    .set("bulletType", bulletType)
    .set("range", range)
    .set("distance", distance)
    .set("damage", damage)
    .set("accuracy", accuracy)
    .set("spawnX", spawnX)
    .set("spawnY", spawnY);
}

void InternalCallPayload::push(lua_State* L) const
{
  LuaTable(L, 2).set("functionName", functionName).set("elementId", elementId);
}

void EmptyPayload::push(lua_State* L) const
{
  lua_pushnil(L);
}
