#pragma once

#include "../../CoreHeaders/Entity/Entity.h"
#include "../Components/ComponentFactory.h"
#include "../Systems/SystemFactory.h"
#include "../Utils/DataUtils/ObjectLoader.h"

#include <lua.hpp>
#include <map>

class EntityBuilder
{
public:
  EntityBuilder() : L(nullptr), _loader(nullptr) {};
  EntityBuilder(lua_State* state, ObjectLoader* loader) : L(state), _loader(loader)
  {
    _sFac = new SystemFactory();
    _cFac = new ComponentFactory();
  };

  void createEntities();

private:
  lua_State* L;
  ObjectLoader* _loader;
  SystemFactory* _sFac;
  ComponentFactory* _cFac;

  void basicInfo(const char* name, Entity& e);
  void modelInfo(Entity& e);
  void createProperties(Entity& e);
  void shaderInfo(Entity& e);
  void parseInfos(std::map<std::string, std::int32_t>& infos, std::string& name);
  Model* loadModel(float* vertices,
                   size_t verticeLength,
                   std::int32_t* indices,
                   size_t indiceLength,
                   float* textureCoords,
                   size_t textureCoordLength);
  Model* loadModel(const char* path);
  Shader* loadShader(const char* vertexShaderFile, const char* fragShaderFile, std::int32_t shaderGroup);
  Texture* loadTexture(const char* textureFile);
};
