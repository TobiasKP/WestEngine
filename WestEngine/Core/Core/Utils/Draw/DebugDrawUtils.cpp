#include "../../../CoreHeaders/Utils/Draw/DebugDrawUtils.h"

#ifdef _WIN32
<include> direct.h
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif

#include "../../../Config/Config.h"
#include "../../../Constants/CoreConstants.h"
#include "../../../CoreHeaders/Entity/Entity.h"

DebugDrawUtils::DebugDrawUtils(WestLogger *logger) {
  _loader = new ObjectLoader(logger);
  _scene = &Scene::getSceneInstance();
}

Entity *DebugDrawUtils::addLine(glm::vec3 start, glm::vec3 direction,
                                glm::vec3 color) {

  glm::vec3 end = start + direction;
  GLfloat vertices[] = {start.x, start.y, start.z, end.x, end.y, end.z};
  std::int32_t indices[] = {0, 1};
  Model *m = _loader->loadModel(vertices, sizeof(vertices), indices,
                                sizeof(indices), 0, 0, 0, 0);
  m->color = (color);

  char cwd[PATH_MAX];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return nullptr;

  Shader *s = new Shader();
  s->vertexShaderFile = CoreConstants::DEBUG_V_SHADER;
  s->fragShaderFile = CoreConstants::DEBUG_F_SHADER;
  s->shadergroup = CoreConstants::DEBUG_SHADERGROUP;

  Entity *e = new Entity(Config::EngineInternals.INTERNAL_ENTITY_ID++);
  e->addComponent(BitMasks::Components::SHADER, s);
  e->addComponent(BitMasks::Components::MODEL, m);
  e->debugEntity();

  _scene->addDebugEntity(e);
  return e;
};

void DebugDrawUtils::unloadModel(Entity *entity) {
  _loader->unloadModel(
      (Model *)entity->getComponent(BitMasks::Components::MODEL));
}
