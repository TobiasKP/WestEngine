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
  void createProperties(Entity& e);
  void shaderInfo(Entity& e);
  void parseInfos(std::map<std::string, std::int32_t>& infos, std::string& name);
  void createWorld(Entity& e);
  std::tuple<Model*, Material*> loadModel(std::string path);
  Shader* loadShader(std::string vertexShaderFile, std::string fragShaderFile, std::int32_t shaderGroup);
};
