#include "../../CoreHeaders/Components/ComponentFactory.h"

#include "../../Constants/Components.hpp"
#include "../../CoreHeaders/Components/Umbrella.h"
#include "../../CoreHeaders/Entity/Scene.h"

#include <format>

void ComponentFactory::createComponent(lua_State* L, std::string& name, Entity& e)
{
#ifdef DEBUG
  WestLogger::getLoggerInstance().log(Level::Info, std::format("Adding component: {} to: {}\n", name, e.getId()));
#endif
  if (Components::MMA_COMBINATION.compare(name) == 0)
  {
    std::string path             = std::format("/assets/Models/{}", lua_tostring(L, 2));
    auto [model, material, aabb] = _loader->loadModel(path);
    _registry->addComponent<Model>(e.getId(), std::move(*model));
    _registry->addComponent<Material>(e.getId(), std::move(*material));
    _registry->addComponent<AABB>(e.getId(), std::move(*aabb));
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
  else if (Components::HEALTH.compare(name) == 0)
  {
    addHealth(L, e);
  }
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
  Scene::getSceneInstance().getWorld()->addEntityIdToIdx(p.position.x, p.position.z, e.getId());
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
  Shader s = {};
  lua_getfield(L, 2, "v");
  s.vertexShaderFile = std::format("/shader/{}", lua_tostring(L, -1));
  lua_pop(L, 1);
  lua_getfield(L, 2, "f");
  s.fragShaderFile = std::format("/shader/{}", lua_tostring(L, -1));
  lua_pop(L, 1);
  lua_getfield(L, 2, "group");
  s.shadergroup = lua_tointeger(L, -1);
  lua_pop(L, 1);
  _registry->addComponent<Shader>(e.getId(), std::move(s));
};
