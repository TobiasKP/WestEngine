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
  glm::vec3 end          = start + direction;
  GLfloat vertices[]     = {start.x, start.y, start.z, end.x, end.y, end.z};
  std::int32_t indices[] = {0, 1};
  std::tuple<Model*, Material*> m =
    _loader->loadModel(vertices, sizeof(vertices), indices, sizeof(indices), 0, 0, 0, 0, "");

  Shader* s           = new Shader();
  s->vertexShaderFile = CoreConstants::DEBUG_V_SHADER;
  s->fragShaderFile   = CoreConstants::DEBUG_F_SHADER;
  s->shadergroup      = CoreConstants::DEBUG_SHADERGROUP;

  Entity e;
  e.setId(Config::incEntityId());
  e.addComponent(BitMasks::Components::SHADER, s);
  e.addComponent(BitMasks::Components::MODEL, std::move(std::get<0>(m)));
  e.addComponent(BitMasks::Components::MATERIAL, std::move(std::get<1>(m)));
  e.debugEntity();

  _scene->addDebugEntity(std::move(e));
  return e.getId();
};

void DebugDrawUtils::unloadModel(const Entity& entity)
{
  _loader->unloadModel((Model*)entity.getComponent(BitMasks::Components::MODEL),
                       (Material*)entity.getComponent(BitMasks::Components::MATERIAL));
}
