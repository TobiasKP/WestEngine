#include "../../CoreHeaders/Entity/WorldBuilder.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../Scripting/LuaFacade.hpp"

#include <format>
#include <WestAssetFacade.hpp>


WorldBuilder::WorldBuilder(lua_State* l, std::shared_ptr<ComponentRegistry> r, std::shared_ptr<Scene> s)
  : _registry(r), _scene(s)
{
  LuaFacade::getLuaFacadeInstance().registerCFunction(loadWorld, LuaAPI::C_LOAD_WORLD.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(setTileBlocked, LuaAPI::C_SET_TILE_BLOCKED.data(), this);
};

WorldBuilder::~WorldBuilder() {}


int WorldBuilder::loadWorld(lua_State* L)
{
  std::int32_t n   = lua_gettop(L);
  WorldBuilder* me = (WorldBuilder*)lua_touserdata(L, lua_upvalueindex(1));
  assert(me != nullptr);
  std::shared_ptr<World> w = std::make_shared<World>();
  w->setId(Config::incEntityId());
  me->createWorld(*w, L);
  me->_scene->addWorld(w);
  return 0;
};

int WorldBuilder::setTileBlocked(lua_State* L)
{
  WorldBuilder* me         = (WorldBuilder*)lua_touserdata(L, lua_upvalueindex(1));
  std::shared_ptr<World> w = me->_scene->getWorld();
  lua_Integer x            = luaL_checkinteger(L, 1);
  lua_Integer y            = luaL_checkinteger(L, 2);
  std::int32_t dimension   = w ? w->getGridSize() : 0;
  if (x < 0 || y < 0 || x >= dimension || y >= dimension)
  {
    WestLogger::getLoggerInstance().log(Level::Error,
                                        std::format("setTileBlocked: tile {}, {} is off the grid\n", x, y));
    return 0;
  }
  w->setFlag(0x0008u, static_cast<std::int32_t>(y * dimension + x));
  return 0;
}

void WorldBuilder::createWorld(World& w, lua_State* L)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Loading World\n"));
#endif
  std::int32_t sqmap;
  std::vector<std::uint8_t> map;
  std::string path = "";
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
      if (lua_isstring(L, -1))
      {
        path = lua_tostring(L, -1);
      }
      else
      {
        lua_pushnil(L);
        while (lua_next(L, -2) > 0)
        {
          map.push_back(lua_tonumber(L, -1));
          lua_pop(L, 1);
        }
      }
      lua_pop(L, 1);
    }
    lua_pop(L, 1);
  }

  sqmap = sqrt(map.size());
  w.setCreationInformation(sqmap, 1, glm::vec2(-sqmap / 2.0f, -sqmap / 2.0f));

#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("World comes with {} skybox\n", path));
#endif


  if (path.size() > 0)
  {
    Model* s                      = buildSkybox(CoreConstants::SKYBOX_PATH.data() + path);
    const std::string& guidSkybox = WestData::WestAssetFacade::getAssetFacade().addModelToScene(*s);
    w.setSkybox(guidSkybox);
  }
  Model* m                = buildWorldMesh(map, sqmap);
  const std::string& guid = WestData::WestAssetFacade::getAssetFacade().addModelToScene(*m);
  w.setModelGuid(guid);
}

Model* WorldBuilder::buildSkybox(const std::string& texturePath)
{
  Model* m = new Model();
  m->setGuid("skybox");
  std::vector<std::uint32_t> idx;
  std::vector<Texture> tex;
  std::vector<Vertex> vert;

  for (std::int32_t side = 0; side < 6; ++side)
  {
    tex.push_back(WestData::WestAssetFacade::getAssetFacade().loadTexture(texturePath + faces[side]));
    std::int32_t base   = static_cast<std::int32_t>(vert.size());
    const glm::vec3& n  = faceBasis[side][0];
    const glm::vec3& tu = faceBasis[side][1];
    const glm::vec3& tv = faceBasis[side][2];

    for (std::int32_t v = 0; v < 4; ++v)
    {
      const float u = baseQuad[v * 2] * 2.0f - 1.0f;
      const float w = baseQuad[v * 2 + 1] * 2.0f - 1.0f;
      vert.push_back(Vertex(n + tu * u + tv * w, -n, glm::vec2(baseQuad[v * 2], baseQuad[v * 2 + 1])));
    }

    for (std::int32_t i = 0; i < 6; ++i)
    {
      idx.push_back(base + static_cast<std::int32_t>(indices[i]));
    }

    Mesh mesh = Mesh(std::format("skybox_{}", side), vert, idx, tex, AABB());
    m->addMesh(std::move(mesh));
    idx.clear();
    tex.clear();
    vert.clear();
  }


  return m;
};

Model* WorldBuilder::buildWorldMesh(const std::vector<std::uint8_t>& map, std::int32_t sqmap)
{
  std::vector<std::uint32_t> idx;
  std::vector<Texture> tex;
  std::vector<Vertex> vert;
  Model* m = new Model();
  m->setGuid("world");

  for (std::int32_t row = 0; row < sqmap; ++row)
  {
    for (std::int32_t col = 0; col < sqmap; ++col)
    {
      std::int32_t base = static_cast<std::int32_t>(vert.size());
      float height      = static_cast<float>(map[row * sqmap + col]);

      for (std::int32_t v = 0; v < 4; ++v)
      {
        vert.push_back(Vertex(glm::vec3((baseQuad[v * 2] + static_cast<float>(col)) - (sqmap >> 1),
                                        height,
                                        (baseQuad[v * 2 + 1] + static_cast<float>(row)) - (sqmap >> 1)),
                              glm::vec3(0),
                              glm::vec2(baseQuad[v * 2], baseQuad[v * 2 + 1])));
      }

      for (std::int32_t i = 0; i < 6; ++i)
      {
        idx.push_back(base + static_cast<std::int32_t>(indices[i]));
      }
    }
  }

  Mesh mesh                  = Mesh("world", vert, idx, tex, AABB());
  mesh.material.diffuseColor = glm::vec3(0.2f, 0.6f, 0.2f);
  m->addMesh(mesh);
  return m;
}
