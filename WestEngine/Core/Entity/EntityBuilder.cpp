#include "../../CoreHeaders/Entity/EntityBuilder.h"

#include "../../Constants/Components.hpp"
#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Entity/Scene.h"

#include <Config.h>
#include <format>
#include <string.h>

void EntityBuilder::createEntities()
{
  lua_pushnil(L);
  while (lua_next(L, -2) > 0)
  {
    Entity e;
    e.setId(Config::incEntityId());
    lua_pushnil(L);
    bool toAdd = true;
    while (lua_next(L, -2) > 0)
    {
      const char* key = lua_tostring(L, -2);
      if (strcmp(key, "shader") == 0)
      {
        shaderInfo(e);
      }
      else if (strcmp(key, "world") == 0)
      {
        World* w = new World();
        w->setId(e.getId());
        createWorld(*w);
        Scene::getSceneInstance().addWorld(w);
        lua_pop(L, 2);
        toAdd = false;
        break;
      }
      else if (strcmp(key, "_parseLog") == 0)
      {
#ifdef DEBUG
        WestLogger::getLoggerInstance().log(Level::Info, std::format("{}\n", lua_tostring(L, -1)));
#endif
      }
      else if (lua_istable(L, -1))
      {
        createProperties(e);
      }
      else
      {
#ifdef DEBUG
        WestLogger::getLoggerInstance().log(Level::Cycle, std::format("Adding new Info: {} to:{}\n", key, e.getId()));
#endif
        basicInfo(key, e);
      }

      lua_pop(L, 1);
    }

    lua_pop(L, 1);

#ifdef DEBUG
    WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding new Entity to Scene:{}\n", e.getId()));
#endif
    if (toAdd)
    {
      Scene::getSceneInstance().addEntity(std::move(e));
    }
  }
}

void EntityBuilder::createProperties(Entity& e)
{
  std::map<std::string, std::int32_t> infos;
  std::string name = CoreConstants::UNDEFINED_STRING.data();
  const char* key  = lua_tostring(L, -2);

  lua_pushnil(L);
  if (Components::COMPONENTS.compare(key) == 0)
  {
    while (lua_next(L, -2) != 0)
    {
      parseInfos(infos, name);
      _cFac->createComponent(infos, name, e);
      lua_pop(L, 1);
    }
  }
  else if (Systems::SYSTEMS.compare(key) == 0)
  {
    while (lua_next(L, -2) != 0)
    {
      parseInfos(infos, name);
      _sFac->createSystem(infos, name, e);
      lua_pop(L, 1);
    }
  }
}

void EntityBuilder::parseInfos(std::map<std::string, std::int32_t>& infos, std::string& name)
{
  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    const char* key = lua_tostring(L, -2);
    if (strcmp(key, "name") == 0)
    {
      name = (char*)lua_tostring(L, -1);
    }
    else
    {
      infos[key] = lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
  }
}

void EntityBuilder::basicInfo(const char* key, Entity& e)
{
  if (strcmp(key, "name") == 0)
  {
    e.setName((char*)lua_tostring(L, -1));
  }
  else if (strcmp(key, "model") == 0)
  {
    std::string mesh     = lua_tostring(L, -1);
    std::string meshPath = std::format("/assets/Models/{}", mesh);

#ifdef DEBUG
    WestLogger::getLoggerInstance().log(Level::Info, std::format("Loading Model: {}\n", meshPath));
#endif
    std::tuple<Model*, Material*> m = loadModel(meshPath);
    assert(std::get<0>(m) != nullptr);
    e.addComponent(BitMasks::Components::MODEL, std::move(std::get<0>(m)));
    e.addComponent(BitMasks::Components::MATERIAL, std::move(std::get<1>(m)));
  }
}

void EntityBuilder::shaderInfo(Entity& e)
{
  std::string vertexPath, fragmentPath;
  std::int32_t group;

  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    if (lua_isnumber(L, -1))
    {
      group = lua_tonumber(L, -1);
      lua_pop(L, 1);
      continue;
    }
    const char* key = lua_tostring(L, -2);
    if (strcmp(key, "v") == 0)
    {
      std::string vertex = lua_tostring(L, -1);
      vertexPath         = std::format("/shader/{}", vertex);
    }
    else if (strcmp(key, "f") == 0)
    {
      std::string frag = lua_tostring(L, -1);
      fragmentPath     = std::format("/shader/{}", frag);
    }
    lua_pop(L, 1);
  }

#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info,
                                      std::format("Loading Shader:\n\t{}\n\t{}\n", vertexPath, fragmentPath));
#endif

  Shader* s = loadShader(vertexPath, fragmentPath, group);
  e.addComponent(BitMasks::Components::SHADER, s);
}

void EntityBuilder::createWorld(World& e)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Loading World\n"));
#endif
  std::string vertexPath = "/shader/worldshader.vs", fragmentPath = "/shader/worldshader.fs";
  std::int32_t group = 1000, sqmap;
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
    if (!strcmp(lua_tostring(L, 0), "grid"))
    {
#ifdef DEBUG
      WestLogger::getLoggerInstance().log(Level::Error, std::format("Error loading World, expected a grid...\n"));
#endif
    }
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
  e.setCreationInformation(sqmap, 1, glm::vec2(0, 0));

  std::tuple<Model*, Material*> m = buildWorldMesh(map, sqmap);
  Material* mat                   = std::get<1>(m);
  mat->diffuseColor               = glm::vec3(0.2f, 0.6f, 0.2f);

  Shader* s = loadShader(vertexPath, fragmentPath, group);
  e.addComponent(BitMasks::Components::SHADER, s);
  e.addComponent(BitMasks::Components::MODEL, std::get<0>(m));
  e.addComponent(BitMasks::Components::MATERIAL, mat);
}

std::tuple<Model*, Material*> EntityBuilder::buildWorldMesh(const std::vector<std::uint8_t>& map, std::int32_t sqmap)
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
        vertices.push_back(baseQuad[v * 2] + static_cast<float>(col));
        vertices.push_back(height);
        vertices.push_back(baseQuad[v * 2 + 1] + static_cast<float>(row));

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
                            "");
}

std::tuple<Model*, Material*> EntityBuilder::loadModel(std::string path)
{
  return _loader->loadModel(path);
}

Shader*
EntityBuilder::loadShader(std::string vertexShaderFile, std::string fragmentShaderFile, std::int32_t shaderGroup)
{
  Shader* s           = new Shader();
  s->vertexShaderFile = vertexShaderFile;
  s->fragShaderFile   = fragmentShaderFile;
  s->shadergroup      = shaderGroup;
  return s;
}
