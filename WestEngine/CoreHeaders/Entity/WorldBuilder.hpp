#pragma once

#include "../Components/ComponentRegistry.hpp"
#include "Scene.h"
#include "World.hpp"

#include <lua.hpp>
#include <WestAssetFacade.hpp>

class WorldBuilder
{
public:
  WorldBuilder(lua_State* l, std::shared_ptr<ComponentRegistry> r, std::shared_ptr<Scene> s);
  ~WorldBuilder();

  static int loadWorld(lua_State*);

private:
  void createWorld(World& w, lua_State* L);
  Model* buildWorldMesh(const std::vector<std::uint8_t>& map, std::int32_t sqmap);

  static constexpr std::uint32_t indices[6] = {0, 1, 3, 1, 2, 3};
  static constexpr float baseQuad[]         = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  std::shared_ptr<ComponentRegistry> _registry;
  std::shared_ptr<Scene> _scene; 
};
