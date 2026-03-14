#pragma once

#include "../Entity/Entity.h"
#include "../Utils/DataUtils/ObjectLoader.h"
#include "ComponentRegistry.hpp"

#include <lua.hpp>

class ComponentFactory
{
public:
  ComponentFactory(ComponentRegistry* r, ObjectLoader* l) : _registry(r), _loader(l) {};
  void createComponent(lua_State* L, std::string& name, Entity& e);

private:
  ComponentRegistry* _registry;
  ObjectLoader* _loader;

  void addPosition(lua_State* L, Entity& e);
  void addMovement(lua_State* L, Entity& e);
  void addShader(lua_State* L, Entity& e);
  void addHealth(lua_State* L, Entity& e);
  void addPlayerControl(lua_State* L, Entity& e);
};
