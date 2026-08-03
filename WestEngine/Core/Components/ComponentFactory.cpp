#include "../../CoreHeaders/Components/ComponentFactory.h"

#include "../../Constants/Components.hpp"
#include "../../CoreHeaders/Components/Umbrella.h"

#include <format>
#include <WestAssetFacade.hpp>
#include <WestRendererFacade.hpp>

void ComponentFactory::createComponent(lua_State* L, std::string& name, Entity& e)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding component: {} to: {}\n", name, e.getId()));
#endif
  if (Components::MMA_COMBINATION.compare(name) == 0)
  {
    addModel(L, e);
  }
  else if (Components::POSITION.compare(name) == 0)
  {
    // TODO: Add rotation and scale
    addPosition(L, e);
  }
  else if (Components::MOVEMENT.compare(name) == 0)
  {
    addMovement(L, e);
  }
  else if (Components::SHADER.compare(name) == 0)
  {
    addShader(L, e);
  }
  else if (Components::CONTROL.compare(name) == 0)
  {
    addPlayerControl(L, e);
  }
  else if (Components::ACTIVE_UNIT.compare(name) == 0)
  {
    e.toggleActivate(true);
  }
  else if (Components::HEALTH.compare(name) == 0)
  {
    addHealth(L, e);
  }
  else if (Components::EQUIPMENT.compare(name) == 0)
  {
    addEquipment(L, e);
  }
  else if (Components::PROJECTILE.compare(name) == 0)
  {
    addProjectile(L, e);
  }
  else
  {
    WestLogger::getLoggerInstance().log(Level::Error, std::format("Unkown Component: {} \n", name));
  }
}

void ComponentFactory::addProjectile(lua_State* L, Entity& e)
{
  Projectile p = {};
  lua_getfield(L, 2, "speed");
  p.speed = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "dmg");
  p.damage = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "destination");
  p.destination = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "hit");
  p.hit = lua_toboolean(L, -1);
  lua_pop(L, 1);
  _registry->addComponent<Projectile>(e.getId(), std::move(p));
};

void ComponentFactory::addEquipment(lua_State* L, Entity& e)
{
  Equipment q           = {};
  Weapon w              = {};
  const auto fillWeapon = [](lua_State* L, Weapon& w)
  {
    lua_getfield(L, -1, "id");
    w.id = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "dmg");
    w.dmg = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "range");
    w.range = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "accuracy");
    w.accuracy = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return w;
  };


  lua_getfield(L, 2, "primary");
  if (lua_istable(L, -1))
  {
    q.primary = fillWeapon(L, w);
    w         = {};
  }
  lua_pop(L, 1);
  lua_getfield(L, 2, "secondary");
  if (lua_istable(L, -1))
  {
    q.secondary = fillWeapon(L, w);
    w           = {};
  }
  lua_pop(L, 1);

  _registry->addComponent<Equipment>(e.getId(), std::move(q));
}

void ComponentFactory::addHealth(lua_State* L, Entity& e)
{
  Health h = {};
  lua_getfield(L, 2, "m");
  h.max = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "c");
  h.current = lua_tointeger(L, -1);
  lua_pop(L, 1);
  _registry->addComponent<Health>(e.getId(), std::move(h));
}

void ComponentFactory::addPlayerControl(lua_State* L, Entity& e)
{
  Control p  = {};
  p.entityId = e.getId();
  _registry->addComponent<Control>(e.getId(), std::move(p));
}

void ComponentFactory::addPosition(lua_State* L, Entity& e)
{
  Position p    = {};
  glm::vec3 pos = glm::vec3();
  lua_getfield(L, 2, "x");
  pos.x = lua_tonumber(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "y");
  pos.y = lua_tonumber(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "z");
  pos.z = lua_tonumber(L, -1);
  lua_pop(L, 1);
  p.position = pos;
  p.scale    = 1.0f;
  p.rotation = glm::vec3(1.0f);
  _registry->addComponent<Position>(e.getId(), std::move(p));
  _registry->addComponent<Appearance>(e.getId(), Appearance{});
};

void ComponentFactory::addMovement(lua_State* L, Entity& e)
{
  Movement m = {};
  lua_getfield(L, 2, "r");
  m.range = lua_tointeger(L, -1);
  lua_pop(L, 1);
  lua_getfield(L, 2, "a");
  m.a = (algorithm)lua_tointeger(L, -1);
  lua_pop(L, 1);
  _registry->addComponent<Movement>(e.getId(), std::move(m));
};


void ComponentFactory::addShader(lua_State* L, Entity& e)
{
  lua_getfield(L, 2, "v");
  std::string vertexShaderFile = std::format("/shader/{}", lua_tostring(L, -1));
  lua_pop(L, 1);
  lua_getfield(L, 2, "f");
  std::string fragShaderFile = std::format("/shader/{}", lua_tostring(L, -1));
  lua_pop(L, 1);
  e.setShaderId(WestRenderer::WestRendererFacade::getRendererFacade().registerShader(vertexShaderFile, fragShaderFile));
};

void ComponentFactory::addModel(lua_State* L, Entity& e)
{
  std::string file        = lua_tostring(L, 2);
  const std::string& guid = WestData::WestAssetFacade::getAssetFacade().addModelToScene(file);
  e.setModelGuid(guid);
}
