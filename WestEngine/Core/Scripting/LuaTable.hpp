#pragma once

#include <concepts>
#include <cstdint>
#include <lua.hpp>
#include <string_view>

class LuaTable
{
public:
  LuaTable(lua_State* L, const std::int32_t fields) : _L(L)
  {
    lua_createtable(_L, 0, fields);
  }

  LuaTable& set(const char* key, const bool value)
  {
    lua_pushboolean(_L, value);
    return assign(key);
  }

  template <typename T>
  requires std::integral<T> && (!std::same_as<T, bool>)
  LuaTable& set(const char* key, const T value)
  {
    lua_pushinteger(_L, static_cast<lua_Integer>(value));
    return assign(key);
  }

  template <typename T>
  requires std::floating_point<T>
  LuaTable& set(const char* key, const T value)
  {
    lua_pushnumber(_L, static_cast<lua_Number>(value));
    return assign(key);
  }

  LuaTable& set(const char* key, const std::string_view value)
  {
    lua_pushlstring(_L, value.data(), value.size());
    return assign(key);
  }

private:
  LuaTable& assign(const char* key)
  {
    lua_setfield(_L, -2, key);
    return *this;
  }

  lua_State* _L;
};
