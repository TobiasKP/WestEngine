#include "../../CoreHeaders/Entity/EntityBuilder.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../../CoreHeaders/Entity/Scene.h"
#include "../Scripting/LuaFacade.hpp"

EntityBuilder::EntityBuilder(lua_State* state, ObjectLoader* loader, ComponentRegistry* r) : _registry(r)
{
  _cFac = new ComponentFactory(r, loader);
  LuaFacade::getLuaFacadeInstance().registerCFunction(createEntity, LuaAPI::C_CREATE_ENTITY.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(addComponent, LuaAPI::C_ADD_COMPONENT.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(buildEntity, LuaAPI::C_BUILD_ENTITY.data(), this);
};


EntityBuilder::~EntityBuilder()
{
  delete _cFac;
}

int EntityBuilder::createEntity(lua_State* L)
{
  EntityBuilder* me = EntityBuilder::retrieveMeFromStack(L);
  me->_e            = Entity{};
  std::string name  = lua_tostring(L, 1);
  me->_e.setId(Config::incEntityId());
  me->_e.setName(name);
  return 0;
}


int EntityBuilder::addComponent(lua_State* L)
{
  EntityBuilder* me     = EntityBuilder::retrieveMeFromStack(L);
  std::string component = lua_tostring(L, 1);
  me->_cFac->createComponent(L, component, me->_e);
  return 0;
}

int EntityBuilder::buildEntity(lua_State* L)
{
  EntityBuilder* me = EntityBuilder::retrieveMeFromStack(L); 
  Scene::getSceneInstance().addEntity(std::move(me->_e));
  return 0;
}

EntityBuilder* EntityBuilder::retrieveMeFromStack(lua_State* L)
{
  EntityBuilder* me = (EntityBuilder*)lua_touserdata(L, lua_upvalueindex(1));
  assert(me != nullptr);
  return me;
}
