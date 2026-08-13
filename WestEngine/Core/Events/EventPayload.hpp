#pragma once

#include <cstdint>
#include <string>
#include <variant>

struct lua_State;

struct MousePayload
{
  double x      = 0.0;
  double y      = 0.0;
  double scroll = 0.0;

  void push(lua_State* L) const;
};

struct KeyboardPayload
{
  std::int32_t key    = 0;
  std::int32_t action = 0;

  void push(lua_State* L) const;
};

struct GamePayload
{
  std::int32_t turn = 0;

  void push(lua_State* L) const;
};

struct AttackPayload
{
  std::uint32_t attacker = 0;
  std::uint32_t target   = 0;

  void push(lua_State* L) const;
};

struct InterfacePayload
{
  std::uint32_t event    = 0;
  std::uint32_t entityId = 0;
  std::string newValue;

  void push(lua_State* L) const;
};

struct ActionFinishedPayload
{
  std::uint32_t entityId = 0;

  void push(lua_State* L) const;
};

struct EntityPayload
{
  std::uint32_t entityId = 0;

  void push(lua_State* L) const;
};

struct EntityCreatedPayload
{
  std::uint32_t entityId = 0;
  bool playable          = false;
  std::int32_t health    = 0;

  void push(lua_State* L) const;
};

struct StateChangePayload
{
  std::uint32_t entityId = 0;
  std::int32_t oldState  = 0;
  std::int32_t newState  = 0;

  void push(lua_State* L) const;
};

struct EntityAttackPayload
{
  std::uint32_t attacker   = 0;
  std::uint32_t target     = 0;
  std::uint32_t bulletType = 0;
  std::uint32_t range      = 0;
  std::uint32_t distance   = 0;
  std::uint32_t damage     = 0;
  float accuracy           = 0.0f;
  float spawnX             = 0.0f;
  float spawnY             = 0.0f;

  void push(lua_State* L) const;
};

struct InternalCallPayload
{
  std::string functionName;
  std::uint32_t elementId = 0;

  void push(lua_State* L) const;
};

struct EmptyPayload
{
  void push(lua_State* L) const;
};

using EventPayload = std::variant<MousePayload,
                                  KeyboardPayload,
                                  GamePayload,
                                  AttackPayload,
                                  EmptyPayload,
                                  InterfacePayload,
                                  ActionFinishedPayload,
                                  EntityPayload,
                                  EntityCreatedPayload,
                                  StateChangePayload,
                                  EntityAttackPayload,
                                  InternalCallPayload>;
