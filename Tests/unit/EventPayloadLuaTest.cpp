#include <Events/EventPayload.hpp>
#include <gtest/gtest.h>
#include <lua.hpp>

#include <fstream>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

// Contract between EventPayload::push (C++) and WestGame/Scripts/Core/WestAPI.lua.
// LuaFacade::emit pushes (eventId, payloadTable) into OnEvent, and the handler
// table in WestAPI.lua reads payload.<field>. A renamed field on either side
// turns into a silent nil in Lua, so every field name is pinned here.

#ifndef WEST_SOURCE_DIR
#error "WEST_SOURCE_DIR must point at the repository root"
#endif

class EventPayloadLuaTest : public ::testing::Test
{
protected:
  lua_State* L = nullptr;

  void SetUp() override
  {
    L = luaL_newstate();
    ASSERT_NE(L, nullptr);
  }

  void TearDown() override
  {
    lua_close(L);
  }

  // Pushes the payload and leaves it on the stack, asserting it adds exactly one value
  template <typename P>
  void push(const P& payload)
  {
    const int before = lua_gettop(L);
    payload.push(L);
    ASSERT_EQ(lua_gettop(L), before + 1) << "push must leave exactly one value on the stack";
  }

  int fieldType(const char* name)
  {
    lua_getfield(L, -1, name);
    int t = lua_type(L, -1);
    lua_pop(L, 1);
    return t;
  }

  lua_Integer intField(const char* name)
  {
    lua_getfield(L, -1, name);
    EXPECT_TRUE(lua_isinteger(L, -1)) << name << " should be a Lua integer";
    lua_Integer v = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return v;
  }

  lua_Number numField(const char* name)
  {
    lua_getfield(L, -1, name);
    EXPECT_EQ(lua_type(L, -1), LUA_TNUMBER) << name << " should be a number";
    lua_Number v = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return v;
  }

  bool boolField(const char* name)
  {
    lua_getfield(L, -1, name);
    EXPECT_EQ(lua_type(L, -1), LUA_TBOOLEAN) << name << " should be a boolean";
    bool v = lua_toboolean(L, -1);
    lua_pop(L, 1);
    return v;
  }

  std::string strField(const char* name)
  {
    lua_getfield(L, -1, name);
    EXPECT_EQ(lua_type(L, -1), LUA_TSTRING) << name << " should be a string";
    std::string v = lua_isstring(L, -1) ? lua_tostring(L, -1) : "";
    lua_pop(L, 1);
    return v;
  }

  // Names of all keys in the table on top of the stack
  std::set<std::string> keys()
  {
    std::set<std::string> out;
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
      if (lua_type(L, -2) == LUA_TSTRING)
      {
        out.insert(lua_tostring(L, -2));
      }
      lua_pop(L, 1);
    }
    return out;
  }
};

// ─── Payloads WestAPI.lua handlers read ──────────────────────

// ENTITY_CREATED -> StateMachine.RegisterEntity(payload.entityId, payload.playable, payload.health)
TEST_F(EventPayloadLuaTest, EntityCreatedPayloadFields)
{
  push(EntityCreatedPayload{.entityId = 17, .playable = true, .health = 80});
  ASSERT_EQ(lua_type(L, -1), LUA_TTABLE);
  EXPECT_EQ(keys(), (std::set<std::string>{"entityId", "playable", "health"}));
  EXPECT_EQ(intField("entityId"), 17);
  EXPECT_TRUE(boolField("playable"));
  EXPECT_EQ(intField("health"), 80);
}

// ENTITY_DESTROYED, ENTITY_RCLICK, TILE_LCLICK, TILE_RCLICK, AI_THINK -> payload.entityId
TEST_F(EventPayloadLuaTest, EntityPayloadFields)
{
  push(EntityPayload{.entityId = 4242});
  ASSERT_EQ(lua_type(L, -1), LUA_TTABLE);
  EXPECT_EQ(keys(), (std::set<std::string>{"entityId"}));
  EXPECT_EQ(intField("entityId"), 4242);
}

// ENTITY_STATE_CHANGE -> TransitionEntityState(payload.entityId, payload.oldState, payload.newState)
TEST_F(EventPayloadLuaTest, StateChangePayloadFields)
{
  push(StateChangePayload{.entityId = 3, .oldState = 1, .newState = 0});
  ASSERT_EQ(lua_type(L, -1), LUA_TTABLE);
  EXPECT_EQ(keys(), (std::set<std::string>{"entityId", "oldState", "newState"}));
  EXPECT_EQ(intField("entityId"), 3);
  EXPECT_EQ(intField("oldState"), 1);
  EXPECT_EQ(intField("newState"), 0);
}

// ENTITY_ATTACK -> payload.attacker/target/bulletType/range/distance/damage/accuracy/spawnX/spawnY
TEST_F(EventPayloadLuaTest, EntityAttackPayloadFields)
{
  push(EntityAttackPayload{.attacker   = 1,
                           .target     = 2,
                           .bulletType = 2,
                           .range      = 5,
                           .distance   = 3,
                           .damage     = 25,
                           .accuracy   = 85.5f,
                           .spawnX     = 4.5f,
                           .spawnY     = 7.25f});
  ASSERT_EQ(lua_type(L, -1), LUA_TTABLE);
  EXPECT_EQ(keys(),
            (std::set<std::string>{
              "attacker", "target", "bulletType", "range", "distance", "damage", "accuracy", "spawnX", "spawnY"}));
  EXPECT_EQ(intField("attacker"), 1);
  EXPECT_EQ(intField("target"), 2);
  EXPECT_EQ(intField("bulletType"), 2);
  EXPECT_EQ(intField("range"), 5);
  EXPECT_EQ(intField("distance"), 3);
  EXPECT_EQ(intField("damage"), 25);
  EXPECT_FLOAT_EQ(static_cast<float>(numField("accuracy")), 85.5f);
  EXPECT_FLOAT_EQ(static_cast<float>(numField("spawnX")), 4.5f);
  EXPECT_FLOAT_EQ(static_cast<float>(numField("spawnY")), 7.25f);
}

// UI_INTERNAL_CALL -> Logic.Internal(payload.functionName, payload.elementId)
TEST_F(EventPayloadLuaTest, InternalCallPayloadFields)
{
  push(InternalCallPayload{.functionName = "endturn", .elementId = 12});
  ASSERT_EQ(lua_type(L, -1), LUA_TTABLE);
  EXPECT_EQ(keys(), (std::set<std::string>{"functionName", "elementId"}));
  EXPECT_EQ(strField("functionName"), "endturn");
  EXPECT_EQ(intField("elementId"), 12);
}

// ENTITY_QUEUE, UI_REFRESH, LEVEL_END -> handlers take no payload
TEST_F(EventPayloadLuaTest, EmptyPayloadPushesNil)
{
  push(EmptyPayload{});
  EXPECT_TRUE(lua_isnil(L, -1));
}

// ─── Payloads only used inside C++ (still pushable) ──────────

TEST_F(EventPayloadLuaTest, RemainingPayloadsPushTheirFields)
{
  push(MousePayload{.x = 1.5, .y = 2.5, .scroll = -5.0});
  EXPECT_EQ(keys(), (std::set<std::string>{"x", "y", "scroll"}));
  EXPECT_DOUBLE_EQ(numField("x"), 1.5);
  EXPECT_DOUBLE_EQ(numField("scroll"), -5.0);
  lua_pop(L, 1);

  push(KeyboardPayload{.key = 65, .action = 1});
  EXPECT_EQ(keys(), (std::set<std::string>{"key", "action"}));
  EXPECT_EQ(intField("key"), 65);
  lua_pop(L, 1);

  push(GamePayload{.turn = 1});
  EXPECT_EQ(keys(), (std::set<std::string>{"turn"}));
  lua_pop(L, 1);

  push(AttackPayload{.attacker = 5, .target = 6});
  EXPECT_EQ(keys(), (std::set<std::string>{"attacker", "target"}));
  lua_pop(L, 1);

  push(InterfacePayload{.event = 1, .entityId = 2, .newValue = "50"});
  EXPECT_EQ(keys(), (std::set<std::string>{"event", "entityId", "newValue"}));
  EXPECT_EQ(strField("newValue"), "50");
  lua_pop(L, 1);

  push(ActionFinishedPayload{.entityId = 9});
  EXPECT_EQ(keys(), (std::set<std::string>{"entityId"}));
  lua_pop(L, 1);
}

TEST_F(EventPayloadLuaTest, LargeUnsignedIdsSurviveTheRoundTrip)
{
  push(EntityPayload{.entityId = 0xFFFFFFF0u});
  EXPECT_EQ(intField("entityId"), static_cast<lua_Integer>(0xFFFFFFF0u));
}

// ─── Drift detector against the real script ──────────────────

// Every `payload.<field>` WestAPI.lua reads must be produced by some payload push.
TEST_F(EventPayloadLuaTest, EveryFieldWestApiReadsIsPushedBySomePayload)
{
  const std::string path = std::string(WEST_SOURCE_DIR) + "/WestGame/Scripts/Core/WestAPI.lua";
  std::ifstream in(path);
  ASSERT_TRUE(in.is_open()) << "cannot read " << path;
  std::stringstream buf;
  buf << in.rdbuf();
  const std::string script = buf.str();

  std::set<std::string> read;
  const std::regex fieldRe(R"(payload\.([A-Za-z_][A-Za-z0-9_]*))");
  for (auto it = std::sregex_iterator(script.begin(), script.end(), fieldRe); it != std::sregex_iterator(); ++it)
  {
    read.insert((*it)[1].str());
  }
  ASSERT_FALSE(read.empty()) << "no payload.<field> reads found, did the handler table move?";

  std::set<std::string> produced;
  auto collect = [&](const auto& payload)
  {
    payload.push(L);
    if (lua_istable(L, -1))
    {
      auto k = keys();
      produced.insert(k.begin(), k.end());
    }
    lua_pop(L, 1);
  };
  std::visit(collect, EventPayload{MousePayload{}});
  collect(KeyboardPayload{});
  collect(GamePayload{});
  collect(AttackPayload{});
  collect(InterfacePayload{});
  collect(ActionFinishedPayload{});
  collect(EntityPayload{});
  collect(EntityCreatedPayload{});
  collect(StateChangePayload{});
  collect(EntityAttackPayload{});
  collect(InternalCallPayload{});

  for (const std::string& field : read)
  {
    EXPECT_TRUE(produced.contains(field)) << "WestAPI.lua reads payload." << field
                                          << " but no EventPayload::push sets it";
  }
}
