#pragma once

#include "../Entity/Entity.h"
#include "ComponentRegistry.hpp"

#include <lua.hpp>

class ComponentFactory
{
public:
  ComponentFactory(std::shared_ptr<ComponentRegistry> r) : _registry(r) {};
  void createComponent(lua_State* L, std::string& name, Entity& e);

private:
  std::shared_ptr<ComponentRegistry> _registry;

  void addModel(lua_State* L, Entity& e);
  void addPosition(lua_State* L, Entity& e);
  void addMovement(lua_State* L, Entity& e);
  void addShader(lua_State* L, Entity& e);
  void addHealth(lua_State* L, Entity& e);
  void addPlayerControl(lua_State* L, Entity& e);
  void addEquipment(lua_State* L, Entity& e);
  void addProjectile(lua_State* L, Entity& e);
};
