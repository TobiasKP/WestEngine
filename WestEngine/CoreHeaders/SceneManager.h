#pragma once

#include "../Core/Scripting/LuaFacade.hpp"
#include "Components/ComponentRegistry.hpp"
#include "Entity/EntityBuilder.hpp"
#include "Entity/Scene.h"
#include "Entity/WorldBuilder.hpp"
#include "Interfaces/IManager.h"
#include "Utils/DataUtils/ObjectLoader.h"

#include <lua.hpp>

class SceneManager : public IManager
{
public:
  SceneManager();
  SceneManager(WestLogger* logger);
  ~SceneManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

private:
  LuaFacade* _facade;
  Scene* _scene;
  ObjectLoader* _loader;
  EntityBuilder* _builder;
  lua_State* L;
  WorldBuilder* _wbuilder;
  ComponentRegistry* _registry;

  // Functions
  void removeEntityFromScene(const Entity& entity);
  void deleteScene();
  void loadNewScene() {};

  static int getHealth(lua_State*);
};
