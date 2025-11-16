#pragma once

#include "Entity/EntityBuilder.h"
#include "Entity/Scene.h"
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
  Scene* _scene;
  ObjectLoader* _loader;
  lua_State* L;
  EntityBuilder* _builder;

  // Functions
  void removeEntityFromScene(Entity* entity);
  void deleteScene();
  void loadNewScene() {};
};
