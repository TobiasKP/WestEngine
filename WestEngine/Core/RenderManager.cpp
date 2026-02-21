#include "../CoreHeaders/RenderManager.h"

#include "../CoreHeaders/Utils/DataUtils/UniformUtils.h"
#include "../CoreHeaders/Utils/Math/PositionCalculation.h"

#include <Config.h>
#include <format>
#include <glm/ext/matrix_clip_space.hpp>

using namespace WestInterface;

GLuint RenderManager::_usedShaderProgram = 0;

RenderManager::RenderManager() : IManager(nullptr)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = nullptr;
}

RenderManager::RenderManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::RENDER_MANAGER);
  _facade = nullptr;
  _scene  = nullptr;
}

RenderManager::~RenderManager() {}

std::int32_t RenderManager::startup()
{
  return 0;
}

void RenderManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif
}

std::int32_t RenderManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene  = &Scene::getSceneInstance();
  _facade = &WestInterfaceFacade::getInterfaceInstance();
  assert(_scene != nullptr && _facade != nullptr);

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### RenderManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void RenderManager::update()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  clearColor();
  renderWorld();
  renderGameEntities();
  renderUserInterfaces();
#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logCycle(std::format("{} ### render time for all entites in scene: {} ms.\n", getName(), res));
#endif
}

void RenderManager::clearColor()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderManager::renderGameEntities()
{
  if (!Config::PAUSE)
  {
    _scene->getCamera()->update();
  }

  // TODO: entities not sorted by shader group, causes redundant glUseProgram switches
  for (const Entity& entity : _scene->getEntities())
  {
    renderMainLoop(entity);
  }
#ifdef DEBUG
  for (const Entity& entity : _scene->getDebugEntities())
  {
    renderMainLoop(entity);
  }
#endif
}

void RenderManager::renderMainLoop(const Entity& entity)
{
  Shader* s = (Shader*)entity.getComponent(BitMasks::Components::SHADER);
  if (!s->initialized)
  {
#ifdef DEBUG
    logDebug(std::format("!!! Entity shader not initialized! Entity: {}\n", entity.getId()));
#endif
    glUseProgram(0);
    _usedShaderProgram = 0;
    return;
  }

  GLuint shaderProgramId = s->programId;
  if (_usedShaderProgram != shaderProgramId)
  {
    glUseProgram(shaderProgramId);
    _usedShaderProgram = shaderProgramId;
#ifdef DEBUG
    GLint linked;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linked);
    assert(linked == GL_TRUE);
#endif
  }


  Model* model       = (Model*)entity.getComponent(BitMasks::Components::MODEL);
  Material* material = (Material*)entity.getComponent(BitMasks::Components::MATERIAL);
  assert(model != nullptr && material != nullptr);
  // TODO: uniforms re-uploaded every frame even if unchanged, add dirty flags to Position/Material
  if (!Config::PAUSE)
  {
    updateUniforms(entity, model, material);
  }

  glBindVertexArray(model->id);
#ifdef DEBUG
  if (entity.isDebugEntity())
  {
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
  }
#else
  glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
#endif
}


void RenderManager::renderWorld()
{
  glEnable(GL_DEPTH_TEST);

  World* world = _scene->getWorld();
  if (world == nullptr)
  {
    return;
  }

  Shader* s = (Shader*)world->getComponent(BitMasks::Components::SHADER);
  if (s == nullptr || !s->initialized)
  {
#ifdef DEBUG
    logCycle(std::format("{} ### World shader not yet initialized, skipping world render.\n", getName()));
#endif
    return;
  }

  GLuint shaderProgramId = s->programId;
  glUseProgram(shaderProgramId);

  Material* mat = (Material*)world->getComponent(BitMasks::Components::MATERIAL);
  if (mat != nullptr && mat->diffuseColorUniform > -1)
  {
    UniformUtils::setUniform(mat->diffuseColorUniform, mat->diffuseColor);
  }

  Model* model = (Model*)world->getComponent(BitMasks::Components::MODEL);
  if (model == nullptr)
  {
#ifdef DEBUG
    logFailure(std::format("{} ### World model is null, skipping world render.\n", getName()));
#endif
    return;
  }

#ifdef DEBUG
  std::uint8_t cycle = getLogger()->getCycleLength();
  if (cycle == 0)
  {
    logCycle(std::format("{} ### Rendering world. VAO: {}, vertices: {}.\n", getName(), model->id, model->vertexCount));
  }
#endif

  std::vector<std::uint32_t> flags = world->getFlagData();
  std::int32_t dimension           = world->getGridSize();
  UniformUtils::setUniform(world->getFlagUniform(), flags);
  UniformUtils::setUniform(world->getGridUniform(), dimension);


  glBindVertexArray(model->id);
  glDrawElements(GL_TRIANGLES, model->vertexCount, GL_UNSIGNED_INT, 0);
  glUseProgram(_usedShaderProgram);
}

void RenderManager::renderUserInterfaces()
{
  assert(Config::interfaceShaderProgram != -1);
  std::vector<ComponentData*> renderData = _facade->getRenderData();
  if (renderData.size() == 0)
  {
    logFailure(std::format("{} ### No render data for interfaces gathered skipping rendering\n", getName()));
    return;
  }

  // TODO: 4 vectors recreated per frame, use GL_STREAM_DRAW or persistent mapped buffers
  const size_t dataSize = renderData.size();
  std::vector<float> instanceOffsets;
  std::vector<float> colors;
  std::vector<float> textCoords;
  std::vector<std::uint32_t> flags;

  instanceOffsets.reserve(dataSize * 4);
  colors.reserve(dataSize * 4);
  textCoords.reserve(dataSize * 4);
  flags.reserve(dataSize);

  GLuint texture = 0;
  for (ComponentData* cd : renderData)
  {
    instanceOffsets.emplace_back(cd->vertices[0]);
    instanceOffsets.emplace_back(cd->vertices[1]);
    instanceOffsets.emplace_back(cd->stretchX);
    instanceOffsets.emplace_back(cd->stretchY);

    colors.emplace_back(cd->colorR);
    colors.emplace_back(cd->colorG);
    colors.emplace_back(cd->colorB);
    colors.emplace_back(cd->colorA);

    flags.emplace_back(cd->flags);

    textCoords.emplace_back(cd->textureCoords[0]);
    textCoords.emplace_back(cd->textureCoords[1]);
    textCoords.emplace_back(cd->textureCoords[2]);
    textCoords.emplace_back(cd->textureCoords[3]);

    if (cd->texture > 0)
    {
      texture = cd->texture;
    }
  }

  if (instanceOffsets.size() <= 0)
  {
    logFailure(std::format("{} ### No instance data for interfaces gathered skipping rendering\n", getName()));
    return;
  }

#ifdef DEBUG
  std::uint8_t cycle = getLogger()->getCycleLength();
  if (cycle == 0)
  {
    logCycle(std::format("{} ### Rendering Interfaces ...\n", getName()));
  }
#endif

  glUseProgram(Config::interfaceShaderProgram);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(_facade->_interfaceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, _facade->_interfaceCOL);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _facade->_interfaceOFFSET);
  glBufferData(GL_ARRAY_BUFFER, instanceOffsets.size() * sizeof(float), instanceOffsets.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _facade->_interfaceFLAGS);
  glBufferData(GL_ARRAY_BUFFER, flags.size() * sizeof(std::uint32_t), flags.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, _facade->_interfaceUV);
  glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(float), textCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glActiveTexture(GL_TEXTURE0);
  UniformUtils::setUniform(Config::interfaceFontTextureUniform, 0);
  glBindTexture(GL_TEXTURE_2D, _facade->_interfaceFONT_TEXTURE_ID);

  glActiveTexture(GL_TEXTURE1);
  UniformUtils::setUniform(Config::interfaceTextureOneUniform, 1);
  glBindTexture(GL_TEXTURE_2D, texture);

  glm::mat4 ortho = glm::ortho(0.0f, (float)Config::GeneralConfig.WIDTH, 0.0f, (float)Config::GeneralConfig.HEIGHT);
  UniformUtils::setUniform(Config::interfaceOrthoUniform, ortho);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderData.size());
  glDisable(GL_BLEND);
  glUseProgram(_usedShaderProgram);
}

void RenderManager::updateUniforms(const Entity& e, Model* model, Material* material)
{
#ifdef DEBUG
  if (e.isDebugEntity())
  {
    UniformUtils::setUniform(model->debugColorUniform, material->diffuseColor);
    assert(model->debugColorUniform != -1);
  }
#endif

  if (!e.isDebugEntity() && material != nullptr && material->diffuseColorUniform > -1)
  {
    UniformUtils::setUniform(material->diffuseColorUniform, material->diffuseColor);
  }

  Texture* t = material->diffuseTexture;
  if (t != nullptr)
  {
    UniformUtils::setUniform(t->uniform, 0);
    assert(t->uniform != -1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t->id);
  }

  Position* p = (Position*)e.getComponent(BitMasks::Components::POSITION);
  if (p != nullptr)
  {
    glm::mat4 transform = PositionCalculation::createTransformationMatrix(p->position, p->rotation, p->scale);
    assert(p->uniform != -1);
    UniformUtils::setUniform(p->uniform, transform);
  }
}
