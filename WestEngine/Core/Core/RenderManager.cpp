#include "../CoreHeaders/RenderManager.h"

#include <Config.h>
#include <TimeUtils.hpp>

#include <glm/ext/matrix_clip_space.hpp>

#include "../CoreHeaders/Utils/DataUtils/UniformUtils.h"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

GLuint RenderManager::_usedShaderProgram = 0;

RenderManager::RenderManager() : IManager(nullptr) {
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene = nullptr;
}

RenderManager::RenderManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene = nullptr;
}

RenderManager::~RenderManager() {}

std::int32_t RenderManager::startup() {
  glGenVertexArrays(1, &_interfaceVAO);
  glGenBuffers(1, &_interfaceVBO);
  glGenBuffers(1, &_interfaceEBO);
  glGenBuffers(1, &_interfaceCOL);
  return 0;
}

void RenderManager::shutdown() {
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
}

std::int32_t RenderManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene = &Scene::getSceneInstance();
  _facade = &WestInterfaceFacade::getInterfaceInstance();
  assert(_scene != nullptr && _facade != nullptr);

  // TODO outsource maybe even into a Model/Entity
  glBindVertexArray(_interfaceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceVBO);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _interfaceEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_facade->indices),
               _facade->indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, _interfaceCOL);
  glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(
      std::format("{} ### RenderManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void RenderManager::update() {
  clearColor();
  renderUserInterfaces();
  renderGameEntities();
}

void RenderManager::clearColor() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderManager::renderGameEntities() {
  for (Entity *entity : _scene->getEntities()) {
    assert(entity != nullptr);
    Shader *s = (Shader *)entity->getComponent(BitMasks::Components::SHADER);
    if (!s->initialized) {
#ifdef DEBUG
      logDebug(std::format("!!! Entity shader not initialized! Entity: {}\n",
                           entity->getId()));
#endif
      glUseProgram(0);
      _usedShaderProgram = 0;
      continue;
    }

    GLuint shaderProgramId = s->programId;
    if (_usedShaderProgram != shaderProgramId) {
      glUseProgram(shaderProgramId);
      _usedShaderProgram = shaderProgramId;
    }

#ifdef DEBUG
    GLint linked;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linked);
    assert(linked == GL_TRUE);
#endif

    Model *model = (Model *)entity->getComponent(BitMasks::Components::MODEL);
    assert(model != nullptr);
    if (!Config::PAUSE) {
      _scene->getCamera()->update();
      updateUniforms(entity, model);
    }

    glBindVertexArray(model->id);
    if (entity->isDebugEntity())
      glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    else
      glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
  }
}

void RenderManager::renderUserInterfaces() {
  assert(Config::interfaceShaderProgram != -1);
  // TODO calculate hash -> if no changes no need to rerender?
  std::vector<ComponentData *> renderData = _facade->getRenderData();
  if (renderData.size() == 0) {
#ifdef DEBUG
    logCycle(std::format(
        "{} ### No render data for interfaces gathered skipping rendering\n",
        getName()));
#endif
    return;
  }

  std::vector<float> vertices;
  std::vector<float> colors;
  for (ComponentData *cd : renderData) {
    vertices.insert(vertices.end(), std::begin(cd->vertices),
                    std::end(cd->vertices));
    colors.insert(colors.end(),
                  {cd->colorR, cd->colorG, cd->colorB, cd->colorA});
  }
  if (vertices.size() <= 0) {
#ifdef DEBUG
    logCycle(std::format(
        "{} ### No vertice data for interfaces gathered skipping rendering\n",
        getName()));
#endif
    return;
  }
  glUseProgram(Config::interfaceShaderProgram);
  glBindBuffer(GL_ARRAY_BUFFER, _interfaceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, _interfaceCOL);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors.data(), GL_STATIC_DRAW);
  glm::mat4 ortho = glm::ortho(0.0f, (float)Config::GeneralConfig.WIDTH, 0.0f,
                               (float)Config::GeneralConfig.HEIGHT);
  UniformUtils::setUniform(Config::interfaceOrthoUniform, ortho);

  glBindVertexArray(_interfaceVAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  glUseProgram(_usedShaderProgram);
}

void RenderManager::updateUniforms(Entity *e, Model *model) {
#ifdef DEBUG
  if (e->isDebugEntity())
    UniformUtils::setUniform(model->debugColorUniform, model->color);
#endif

  Texture *t = model->texture;
  if (t != nullptr) {
    UniformUtils::setUniform(t->uniform, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t->id);
  }

  Position *p = (Position *)e->getComponent(BitMasks::Components::POSITION);
  if (p != nullptr) {
    glm::mat4 transform = PositionCalculation::createTransformationMatrix(
        p->position, p->rotation, p->scale);
    assert(p->uniform != -1);
    UniformUtils::setUniform(p->uniform, transform);
  }
}
