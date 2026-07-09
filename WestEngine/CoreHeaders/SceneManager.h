#pragma once

#include "../Core/Events/EventDispatcher.hpp"
#include "../Core/Scripting/LuaFacade.hpp"
#include "Components/ComponentRegistry.hpp"
#include "Entity/EntityBuilder.hpp"
#include "Entity/Scene.h"
#include "Entity/WorldBuilder.hpp"
#include "Interfaces/IManager.h"

#include <lua.hpp>
#include <WestAssetFacade.hpp>

class SceneManager : public IManager
{
public:
  SceneManager();
  SceneManager(WestLogger* logger, const std::shared_ptr<EventDispatcher>& d, const std::shared_ptr<Scene>& s);
  ~SceneManager() override;

  // Overrides
  std::int32_t startup() override;
  void shutdown() override;
  void update() override;
  std::int32_t init() override;

private:
  LuaFacade* _facade;

  WestData::WestAssetFacade* _dataFacade; 
  lua_State* L;

  std::unique_ptr<WorldBuilder> _wbuilder;
  std::unique_ptr<EntityBuilder> _ebuilder;

  std::shared_ptr<Scene> _scene;
  std::shared_ptr<ComponentRegistry> _registry;
  std::shared_ptr<EventDispatcher> _dispatcher;

  // Functions
  void removeEntityFromScene(const Entity& entity);
  void deleteScene();
  void loadNewScene() {};

  static int getHealth(lua_State*);
  static int getPosition(lua_State*);
};
