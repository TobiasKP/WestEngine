#include "../../CoreHeaders/Entity/EntityBuilder.h"

#include "../../Constants/Components.hpp"
#include "../../Constants/Systems.hpp"
#include "../../CoreHeaders/Entity/Scene.h"

#include <format>
#include <iostream>
#include <stdio.h>
#include <string.h>

void EntityBuilder::createEntities()
{
  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    Entity* e = new Entity();
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
    WestLogger::getLoggerInstance().log(Level::Info, "Adding new Entity to Scene\n");
#endif
    Scene::getSceneInstance().addEntity(e);
  }
}

void EntityBuilder::createProperties(Entity* e)
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

void EntityBuilder::basicInfo(const char* key, Entity* e)
{
  if (strcmp(key, "id") == 0)
  {
    e->setId(lua_tonumber(L, -1));
  }
  else if (strcmp(key, "name") == 0)
  {
    e->setName((char*)lua_tostring(L, -1));
  }
}

void EntityBuilder::modelInfo(Entity* e)
{
  char meshPath[256] = {0};
  char texPath[256]  = {0};

  strcpy(meshPath, "/assets/Models/");
  strcpy(texPath, "/assets/Textures/");

  lua_pushnil(L);
  while (lua_next(L, -2) != 0)
  {
    const char* key = lua_tostring(L, -2);
    if (strcmp(key, "mesh") == 0)
    {
      const char* mesh = lua_tostring(L, -1);
      strcat(meshPath, mesh);
    }
    if (strcmp(key, "texture") == 0)
    {
      const char* tex = lua_tostring(L, -1);
      if (strlen(tex) > 0)
      {
        strcat(texPath, tex);
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

  Model* m = loadModel(strdup(meshPath));
  if (strlen(texPath) > 0)
  {
    Texture* t = loadTexture(strdup(texPath));
    m->texture = t;
  }

  e->addComponent(BitMasks::Components::MODEL, m);
}

void EntityBuilder::shaderInfo(Entity* e)
{
  char vertexPath[256]   = {0};
  char fragmentPath[256] = {0};

  strncpy(vertexPath, "/shader/", sizeof(vertexPath));
  strncpy(fragmentPath, "/shader/", sizeof(fragmentPath));

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
      const char* vertex = lua_tostring(L, -1);
      strncat(vertexPath, vertex, sizeof(vertexPath) - strlen(vertexPath) - 1);
    }
    else if (strcmp(key, "f") == 0)
    {
      const char* frag = lua_tostring(L, -1);
      strncat(fragmentPath, frag, sizeof(fragmentPath) - strlen(fragmentPath) - 1);
    }
    lua_pop(L, 1);
  }

#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info,
                                      std::format("Loading Shader:\n\t{}\n\t{}\n", vertexPath, fragmentPath));
#endif

  Shader* s = loadShader(strdup(vertexPath), strdup(fragmentPath), group);
  e->addComponent(BitMasks::Components::SHADER, s);
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

Model* EntityBuilder::loadModel(const char* path)
{
  return _loader->loadModel(path);
}

Texture* EntityBuilder::loadTexture(const char* textureFile)
{
  GLuint id  = _loader->loadTexture(textureFile);
  Texture* t = new Texture();
  t->id      = id;
  return t;
}

Shader*
EntityBuilder::loadShader(const char* vertexShaderFile, const char* fragmentShaderFile, std::int32_t shaderGroup)
{
  Shader* s           = new Shader();
  s->vertexShaderFile = (char*)vertexShaderFile;
  s->fragShaderFile   = (char*)fragmentShaderFile;
  s->shadergroup      = shaderGroup;
  return s;
}
