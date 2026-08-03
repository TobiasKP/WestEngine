#include "../CoreHeaders/ShaderManager.h"

#include "../CoreHeaders/Components/Umbrella.h"

#include <Config.h>
#include <format>
#include <PathUtils.h>
#include <stdlib.h>
#include <UniformConstants.hpp>
#include <WestAssetFacade.hpp>
#ifndef _WIN32
#include <limits.h>
#endif
using namespace WestInterface;

ShaderManager::ShaderManager() : IManager(nullptr)
{
  setName(CoreConstants::SHADER_MANAGER);
  _facade = nullptr;
    _scene  = nullptr;
}

ShaderManager::ShaderManager(WestLogger* logger, const std::shared_ptr<Scene>& s) : IManager(logger)
{
  setName(CoreConstants::SHADER_MANAGER);
  _facade = nullptr;
  _scene  = s;
}

ShaderManager::~ShaderManager() {}

std::int32_t ShaderManager::startup()
{
  return 0;
}

void ShaderManager::shutdown()
{
#ifdef DEBUG
  logDebug(std::format("{} ### Shutting down {}...\n", getName(), getName()));
#endif

  glUseProgram(0);
  std::shared_ptr<ComponentRegistry> reg = _scene->getRegistry();
  for (auto& entity : _scene->getEntities())
  {
    GLuint programId = entity.getShaderId();
    glDeleteProgram(programId);
  }
#ifdef DEBUG
  for (auto& entity : _scene->getDebugEntities())
  {
    GLuint programId = entity.getShaderId();
    glDeleteProgram(programId);
  }
#endif
}

std::int32_t ShaderManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _facade         = &WestInterfaceFacade::getInterfaceInstance();
  _rendererFacade = &WestRenderer::WestRendererFacade::getRendererFacade();
  GLuint success  = initInterfaceShader();
  if (success == 1)
  {
    return 1;
  }

  assert(_facade != nullptr && _scene != nullptr);
#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  logDebug(std::format("{} ### ShaderManager init time: {} ms.\n", getName(), res));
#endif

  return 0;
}

void ShaderManager::update()
{
  for (Entity& entity : _scene->getEntities())
  {
    initEntityShader(entity);
  }

#ifdef DEBUG
  for (Entity& entity : _scene->getDebugEntities())
  {
    initEntityShader(entity);
  }
#endif

  initWorldShader();
}

void ShaderManager::initEntityShader(Entity& entity)
{
  if (entity.isInitialized())
  {
    return;
  }

  GLuint programId = entity.getShaderId();
  if (programId == -1)
  {
    logFailure(std::format("{} ### Could not create Shader for entity: {}.\n", getName(), entity.getId()));
    return;
  }

  addUniforms(programId, entity);
  entity.initialize();
}


void ShaderManager::initWorldShader()
{
  std::shared_ptr<World> world = _scene->getWorld();

  if (world == nullptr || world->isInitialized())
  {
    return;
  }

  std::shared_ptr<ComponentRegistry> reg = _scene->getRegistry();

#ifdef DEBUG
  logDebug(std::format("{} ### Initializing world shader.\n", getName()));
#endif


  world->setShaderId(_rendererFacade->registerShader("/shader/worldshader.vs", "/shader/worldshader.fs"));
  world->initialize();
#ifdef DEBUG
  logDebug(std::format("{} ### World shader initialized. ProgramID: {}.\n", getName(), world->getShaderId()));
#endif


  _rendererFacade->createUniform(UniformConstants::DCOLOR, world->getShaderId(), world->getId());
  _rendererFacade->createUniform(UniformConstants::WORLD_TILEARRAY, world->getShaderId(), world->getId());
  _rendererFacade->createUniform(UniformConstants::WORLD_GRIDSIZE, world->getShaderId(), world->getId());
  _rendererFacade->createUniform(UniformConstants::WORLD_GRID_ORIGIN, world->getShaderId(), world->getId());
}


GLuint ShaderManager::initInterfaceShader()
{
  GLuint programId = WestRenderer::WestRendererFacade::getRendererFacade().registerShader(
    std::string(WestInterfaceFacade::interfaceVertexShader),
    std::string(WestInterfaceFacade::interfaceFragementShader));

#ifdef DEBUG
  logDebug(std::format("{} ### Created Shader for Interfaces. ProgramID: {}.\n", getName(), programId));
#endif


  Config::interfaceOrthoUniform       = UniformUtils::createUniform(UniformConstants::ORTHO_UNIFORM, programId);
  Config::interfaceFontTextureUniform = UniformUtils::createUniform(UniformConstants::FONT_TEXTURE_SAMPLER, programId);
  Config::interfaceTextureOneUniform  = UniformUtils::createUniform(UniformConstants::TEXTURE_SAMPLER, programId);
  Config::interfaceShaderProgram      = programId;
  return 0;
}

void ShaderManager::addUniforms(GLuint programId, const Entity& entity)
{
  const Model* m = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(entity.getModelGuid());
  //_rendererFacade->createUniform(UniformConstants::TEXTURE_SAMPLER, programId, entity.getId());
  _rendererFacade->createUniform(UniformConstants::COLOR, programId, entity.getId());
  _rendererFacade->createUniform(UniformConstants::ECOLOR, programId, entity.getId());

#ifdef DEBUG
  if (m != nullptr && entity.isDebugEntity())
  {
    _rendererFacade->createUniform(UniformConstants::DCOLOR, programId, entity.getId());
  }
#endif

  std::shared_ptr<ComponentRegistry> reg = _scene->getRegistry();
  Position* pos                          = reg->getComponent<Position>(entity.getId());
  if (pos != nullptr)
  {
    _rendererFacade->createUniform(UniformConstants::TRANSFORMATION_MATRIX, programId, entity.getId());
  }
}
