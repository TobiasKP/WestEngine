#include "../../../CoreHeaders/Utils/Draw/DebugDrawUtils.h"

#include "../../../CoreHeaders/Entity/Entity.h"

#include <Config.h>
#include <WestAssetFacade.hpp>

DebugDrawUtils::DebugDrawUtils(WestLogger* logger, std::shared_ptr<Scene> s)
{ 
  _scene  = s;
}

std::uint32_t DebugDrawUtils::addLine(glm::vec3 start, glm::vec3 direction)
{
  glm::vec3 end                = start + direction;
  std::vector<Vertex> vertices = {Vertex{start, glm::vec3(0), glm::vec2(0)}, Vertex{end, glm::vec3(0), glm::vec2(0)}};
  std::vector<std::uint32_t> indices = {0, 1};
  std::vector<Texture> texs;
  Model model = {};
  model.addMesh(Mesh(std::format("{}_{}", "debug", 0), vertices, indices, texs));

  Shader* s           = new Shader();
  s->vertexShaderFile = CoreConstants::DEBUG_V_SHADER;
  s->fragShaderFile   = CoreConstants::DEBUG_F_SHADER;
  s->shadergroup      = CoreConstants::DEBUG_SHADERGROUP;

  Entity e;
  e.setId(Config::incEntityId());
  std::shared_ptr<ComponentRegistry> reg = _scene->getRegistry();
  reg->addComponent<Shader>(e.getId(), std::move(*s));

  e.debugEntity();

  _scene->addDebugEntity(std::move(e));
  return e.getId();
};

void DebugDrawUtils::unloadModel(const Entity& entity)
{
  std::shared_ptr<ComponentRegistry> reg = _scene->getRegistry();
}
