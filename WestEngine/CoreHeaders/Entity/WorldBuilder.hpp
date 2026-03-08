#pragma once

#include "../Components/ComponentRegistry.hpp"
#include "../Utils/DataUtils/ObjectLoader.h"
#include "World.hpp"

#include <lua.hpp>


class WorldBuilder
{
public:
  WorldBuilder(ComponentRegistry* r, ObjectLoader* o, lua_State* l);
  ~WorldBuilder();

  static int loadWorld(lua_State*);

private:
  void createWorld(World& w, lua_State* L);
  std::tuple<std::unique_ptr<Model>, std::unique_ptr<Material>, std::unique_ptr<AABB>>
  buildWorldMesh(const std::vector<std::uint8_t>& map, std::int32_t sqmap);

  static constexpr std::uint32_t indices[6] = {0, 1, 3, 1, 2, 3};
  static constexpr float baseQuad[]         = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};

  ComponentRegistry* _registry;
  ObjectLoader* _loader;
};
