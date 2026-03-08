#pragma once

#include "../Components/ComponentFactory.h"
#include "../Components/ComponentRegistry.hpp"
#include "Entity.h"

#include <lua.hpp>

class EntityBuilder
{
public:
  EntityBuilder(lua_State* state, ObjectLoader* loader, ComponentRegistry* r);
  ~EntityBuilder();

  static int createEntity(lua_State*);
  static int addModel(lua_State*);
  static int addShader(lua_State*);
  static int addSystem(lua_State*);
  static int addComponent(lua_State*);
  static int buildEntity(lua_State*);

private:
  Entity _e;

  ComponentFactory* _cFac;
  ComponentRegistry* _registry;

  static EntityBuilder* retrieveMeFromStack(lua_State*);
};
