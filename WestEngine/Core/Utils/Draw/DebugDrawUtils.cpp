#include "../../../CoreHeaders/Utils/Draw/DebugDrawUtils.h"

#include "../../../CoreHeaders/Entity/Entity.h"

#include <Config.h>

DebugDrawUtils::DebugDrawUtils(WestLogger* logger)
{
  _loader = new ObjectLoader(logger);
  _scene  = &Scene::getSceneInstance();
}

std::uint32_t DebugDrawUtils::addLine(glm::vec3 start, glm::vec3 direction)
{
  glm::vec3 end                = start + direction;
  GLfloat vertices[]           = {start.x, start.y, start.z, end.x, end.y, end.z};
  std::int32_t indices[]       = {0, 1};
  auto [model, material, aabb] = _loader->loadModel(
    vertices, sizeof(vertices), indices, sizeof(indices), 0, 0, 0, 0, "", glm::vec3(0), glm::vec3(0));

  Shader* s           = new Shader();
  s->vertexShaderFile = CoreConstants::DEBUG_V_SHADER;
  s->fragShaderFile   = CoreConstants::DEBUG_F_SHADER;
  s->shadergroup      = CoreConstants::DEBUG_SHADERGROUP;

  Entity e;
  e.setId(Config::incEntityId());
  ComponentRegistry* reg = _scene->getRegistry();
  reg->addComponent<Shader>(e.getId(), std::move(*s));
  reg->addComponent<Model>(e.getId(), std::move(*model));
  reg->addComponent<Material>(e.getId(), std::move(*material));
  e.debugEntity();

  _scene->addDebugEntity(std::move(e));
  return e.getId();
};

void DebugDrawUtils::unloadModel(const Entity& entity)
{
  ComponentRegistry* reg = _scene->getRegistry();
  _loader->unloadModel(reg->getComponent<Model>(entity.getId()), reg->getComponent<Material>(entity.getId()));
}
