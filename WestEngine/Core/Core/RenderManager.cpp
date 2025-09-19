#include "../CoreHeaders/RenderManager.h"

#include <Config.h>
#include <TimeUtils.hpp>

#include "../CoreHeaders/Utils/DataUtils/ObjectLoader.h"
#include "../CoreHeaders/Utils/DataUtils/UniformUtils.h"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

GLuint RenderManager::_usedShaderProgram = 0;

RenderManager::RenderManager() : IManager(nullptr) {
  setName(CoreConstants::RENDER_MANAGER);
}

RenderManager::RenderManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::RENDER_MANAGER);
}

RenderManager::~RenderManager() {}

std::int32_t RenderManager::startup() { return 0; }

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
  // renderUserInterfaces();
  renderGameEntities();
}

void RenderManager::clearColor() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderManager::renderGameEntities() {
  for (Entity *entity : _scene->getEntities()) {
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
  // glUseProgram(Global::UserInterface::SHADER_PROGRAM);
  //_usedShaderProgram = Global::UserInterface::SHADER_PROGRAM;
  glBindVertexArray(_quadVAO);
  UniformUtils::setUniform(_texture, 0);
  // UniformUtils::setUniform(Global::UserInterface::ORTHO_UNIFORM,
  //                          Global::UserInterface::ORTHO_MATRIX);
  std::vector<glm::vec2> positionsOfElements;
  std::uint16_t totalCount;
  /*for (IUserInterface *interface : InterfaceManager::getInterfaces()) {
    if (!interface->isVisible())
      continue;
    for (IUserInterfaceElement *element : interface->getElements()) {
      glm::vec2 pos = element->getPosition();
      GLuint tiles = element->getNumberOfTiles();
      totalCount += tiles;
      for (std::uint8_t i = 0; i < tiles; i++) {
        positionsOfElements.emplace_back(pos);
      }

      // TODO get all informations for a single transmit to the GPU
      // multithreaded?
    }
  }*/

  glBindBuffer(GL_ARRAY_BUFFER, _POS);
  glInvalidateBufferData(_POS);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positionsOfElements.size(),
               positionsOfElements.data(), GL_DYNAMIC_DRAW);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texture);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, totalCount);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
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
