#pragma once

#include "../Components/ComponentFactory.h"
#include "../Components/ComponentRegistry.hpp"
#include "Entity.h"
#include "Scene.h"

#include <lua.hpp>

class EntityBuilder
{
public:
  EntityBuilder(lua_State* state, std::shared_ptr<ComponentRegistry> r, std::shared_ptr<Scene> s);
  ~EntityBuilder();

  static int createEntity(lua_State*);
  static int addModel(lua_State*);
  static int addShader(lua_State*);
  static int addSystem(lua_State*);
  static int addComponent(lua_State*);
  static int buildEntity(lua_State*);

private:
  Entity _e;

  std::unique_ptr<ComponentFactory> _cFac;
  std::shared_ptr<ComponentRegistry> _registry;
  std::shared_ptr<Scene> _scene;

  static EntityBuilder* retrieveMeFromStack(lua_State*);
};
