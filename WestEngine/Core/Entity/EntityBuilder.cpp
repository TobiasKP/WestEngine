#include "../../CoreHeaders/Entity/EntityBuilder.hpp"

#include "../../Constants/LuaAPI.hpp"
#include "../Scripting/LuaFacade.hpp"

#include <format>

EntityBuilder::EntityBuilder(lua_State* state, std::shared_ptr<ComponentRegistry> r, std::shared_ptr<Scene> s)
  : _registry(r), _scene(s)
{
  _cFac = std::make_unique<ComponentFactory>(r);
  LuaFacade::getLuaFacadeInstance().registerCFunction(createEntity, LuaAPI::C_CREATE_ENTITY.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(addComponent, LuaAPI::C_ADD_COMPONENT.data(), this);
  LuaFacade::getLuaFacadeInstance().registerCFunction(buildEntity, LuaAPI::C_BUILD_ENTITY.data(), this);
};


EntityBuilder::~EntityBuilder()
{
  _cFac.reset();
}

int EntityBuilder::createEntity(lua_State* L)
{
  EntityBuilder* me = EntityBuilder::retrieveMeFromStack(L);
  me->_e            = Entity{};
  std::string name  = lua_tostring(L, 1);
  me->_e.setId(Config::incEntityId());
  me->_e.setName(name);
  me->_e.toggleActivate(false);
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
  if (me->_e.getModelGuid().empty())
  {
    WestLogger::getLoggerInstance().log(
      Level::Error, std::format("Error creating Entity: {}, model failed to load, skipping.\n", me->_e.getName()));
    me->_registry->removeAllComponents(me->_e.getId());
    me->_e = {};
    return 0;
  }
  Position* p       = me->_registry->getComponent<Position>(me->_e.getId());
  Control* c        = me->_registry->getComponent<Control>(me->_e.getId());
  Health* h         = me->_registry->getComponent<Health>(me->_e.getId());
  bool playable     = c != nullptr && c->aiControl == false ? true : false;
  if (c != nullptr)
  {
    assert(h != nullptr);
    bool result = LuaFacade::getLuaFacadeInstance().emit(
      EventIdentifiers::ENTITY_CREATED,
      EntityCreatedPayload{.entityId = me->_e.getId(), .playable = playable, .health = h->current});
    if (result != 0)
    {
      WestLogger::getLoggerInstance().log(
        Level::Error, std::format("Error creating Entity, can not add to lua registration aborting scene addition."));
      me->_e = {};
      return 1;
    }
  }

  if (me->_e.isActiveUnit())
  {
    me->_scene->getWorld()->addEntityIdToIdx(p->position.x, p->position.z, me->_e.getId());
  }
  me->_scene->addEntity(std::move(me->_e));
  return 0;
}

EntityBuilder* EntityBuilder::retrieveMeFromStack(lua_State* L)
{
  EntityBuilder* me = (EntityBuilder*)lua_touserdata(L, lua_upvalueindex(1));
  assert(me != nullptr);
  return me;
}
