#include "../../CoreHeaders/Entity/WorldBuilder.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../../CoreHeaders/Entity/Scene.h"
#include "../Scripting/LuaFacade.hpp"

#include <format>


WorldBuilder::WorldBuilder(ComponentRegistry* r, ObjectLoader* o, lua_State* l) : _registry(r), _loader(o)
{
  LuaFacade::getLuaFacadeInstance().registerCFunction(loadWorld, LuaAPI::C_LOAD_WORLD.data(), this);
};

WorldBuilder::~WorldBuilder() {}


int WorldBuilder::loadWorld(lua_State* L)
{
  std::int32_t n   = lua_gettop(L);
  WorldBuilder* me = (WorldBuilder*)lua_touserdata(L, lua_upvalueindex(1));
  assert(me != nullptr);

  World* w = new World();
  w->setId(Config::incEntityId());
  me->createWorld(*w, L);
  Scene::getSceneInstance().addWorld(w);
  return 0;
};

void WorldBuilder::createWorld(World& w, lua_State* L)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Loading World\n"));
#endif
  std::int32_t sqmap;
  std::vector<std::uint8_t> map;
  lua_pushnil(L);
  if (!lua_istable(L, -2))
  {
#ifdef DEBUG
    WestLogger::getLoggerInstance().log(Level::Error, std::format("Error loading World, expected a table...\n"));
#endif
    return;
  }
  while (lua_next(L, -2) > 0)
  {
    lua_pushnil(L);
    while (lua_next(L, -2) > 0)
    {
      lua_pushnil(L);
      while (lua_next(L, -2) > 0)
      {
        map.push_back(lua_tonumber(L, -1));
        lua_pop(L, 1);
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);
  }

  sqmap = sqrt(map.size());
  w.setCreationInformation(sqmap, 1, glm::vec2(-sqmap / 2.0f, -sqmap / 2.0f));

  auto [model, material, aabb] = buildWorldMesh(map, sqmap);
  material->diffuseColor       = glm::vec3(0.2f, 0.6f, 0.2f);

  Shader s           = {};
  s.vertexShaderFile = "/shader/worldshader.vs";
  s.fragShaderFile   = "/shader/worldshader.fs";
  s.shadergroup      = 1000;
  _registry->addComponent<Shader>(w.getId(), std::move(s));
  _registry->addComponent<Model>(w.getId(), std::move(*model));
  _registry->addComponent<Material>(w.getId(), std::move(*material));
}

std::tuple<std::unique_ptr<Model>, std::unique_ptr<Material>, std::unique_ptr<AABB>>
WorldBuilder::buildWorldMesh(const std::vector<std::uint8_t>& map, std::int32_t sqmap)
{
  std::vector<float> vertices;
  std::vector<float> texCoords;
  std::vector<std::int32_t> idx;

  for (std::int32_t row = 0; row < sqmap; ++row)
  {
    for (std::int32_t col = 0; col < sqmap; ++col)
    {
      std::int32_t base = static_cast<std::int32_t>(vertices.size() / 3);
      float height      = static_cast<float>(map[row * sqmap + col]);

      for (int v = 0; v < 4; ++v)
      {
        vertices.push_back((baseQuad[v * 2] + static_cast<float>(col)) - (sqmap >> 1));
        vertices.push_back(height);
        vertices.push_back((baseQuad[v * 2 + 1] + static_cast<float>(row)) - (sqmap >> 1));

        texCoords.push_back(baseQuad[v * 2]);
        texCoords.push_back(baseQuad[v * 2 + 1]);
      }

      for (int i = 0; i < 6; ++i)
      {
        idx.push_back(base + static_cast<std::int32_t>(indices[i]));
      }
    }
  }

  return _loader->loadModel(vertices.data(),
                            vertices.size() * sizeof(float),
                            idx.data(),
                            idx.size() * sizeof(std::int32_t),
                            texCoords.data(),
                            texCoords.size() * sizeof(float),
                            nullptr,
                            0,
                            "",
                            glm::vec3(0),
                            glm::vec3(0));
}
