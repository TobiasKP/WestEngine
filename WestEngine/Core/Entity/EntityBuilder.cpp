#include "../../CoreHeaders/Entity/EntityBuilder.h"

#include "../../Constants/Components.hpp"
#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Entity/Scene.h"

#include <Config.h>
#include <format>
#include <stdio.h>
#include <string.h>

void EntityBuilder::createEntities()
{
  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    Entity e;
    e.setId(Config::incEntityId());
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
      const char* key = lua_tostring(L, -2);
      if (strcmp(key, "model") == 0)
      {
        modelInfo(e);
      }
      else if (strcmp(key, "shader") == 0)
      {
        shaderInfo(e);
      }
      else if (lua_istable(L, -1))
      {
        createProperties(e);
      }
      else
      {
        basicInfo(key, e);
      }

      lua_pop(L, 1);
    }

    lua_pop(L, 1);

#ifdef DEBUG
    WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding new Entity to Scene:{}\n", e.getId()));
#endif
    Scene::getSceneInstance().addEntity(std::move(e));
  }
}

void EntityBuilder::createProperties(Entity& e)
{
  std::map<std::string, std::int32_t> infos;
  std::string name = CoreConstants::UNDEFINED_STRING;
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
}

void EntityBuilder::modelInfo(Entity& e)
{
  std::string meshPath, texPath;

  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    const char* key = lua_tostring(L, -2);
    if (strcmp(key, "mesh") == 0)
    {
      std::string mesh = lua_tostring(L, -1);
      meshPath         = std::format("/assets/Models/{}", mesh);
    }
    if (strcmp(key, "texture") == 0)
    {
      std::string tex = lua_tostring(L, -1);
      if (tex.length() > 0)
      {
        texPath = std::format("/assets/Models/{}", tex);
      }
      else
      {
        texPath[0] = '\0';
      }
    }
    lua_pop(L, 1);
  }

#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Loading Model: {}\n", meshPath));
#endif

  Model* m = loadModel(meshPath);
  if (texPath.length() > 0)
  {
    Texture* t = loadTexture(texPath);
    m->texture = t;
  }

  e.addComponent(BitMasks::Components::MODEL, m);
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

Model* EntityBuilder::loadModel(float* vertices,
                                size_t verticeLength,
                                std::int32_t* indices,
                                size_t indiceLength,
                                float* textureCoords,
                                size_t textureCoordLength)
{
  return _loader->loadModel(vertices, verticeLength, indices, indiceLength, textureCoords, textureCoordLength, 0, 0);
}

Model* EntityBuilder::loadModel(std::string path)
{
  return _loader->loadModel(path);
}

Texture* EntityBuilder::loadTexture(std::string textureFile)
{
  GLuint id  = _loader->loadTexture(textureFile);
  Texture* t = new Texture();
  t->id      = id;
  return t;
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
