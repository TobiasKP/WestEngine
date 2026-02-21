#include "../CoreHeaders/ShaderManager.h"

#include "../Constants/UniformConstants.hpp"
#include "../CoreHeaders/Utils/DataUtils/UniformUtils.h"

#include <Config.h>
#include <format>
#include <PathUtils.h>
#include <stdio.h>
#include <stdlib.h>

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

ShaderManager::ShaderManager(WestLogger* logger) : IManager(logger)
{
  setName(CoreConstants::SHADER_MANAGER);
  _facade = nullptr;
  _scene  = nullptr;
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
  for (auto& entity : _scene->getEntities())
  {
    Shader* s        = (Shader*)entity.getComponent(BitMasks::Components::SHADER);
    GLuint programId = s->programId;
    glDeleteProgram(programId);
  }
#ifdef DEBUG
  for (auto& entity : _scene->getDebugEntities())
  {
    Shader* s        = (Shader*)entity.getComponent(BitMasks::Components::SHADER);
    GLuint programId = s->programId;
    glDeleteProgram(programId);
  }
#endif
}

std::int32_t ShaderManager::init()
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _facade = &WestInterfaceFacade::getInterfaceInstance();
  _scene  = &Scene::getSceneInstance();
  _scene->getCamera()->setCameraUniforms(
    UniformUtils::createUniformBufferObject(UniformConstants::CAMERA_UNIFORMS, sizeof(glm::mat4) * 2, 1));
  GLuint success = initInterfaceShader();
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
  for (const Entity& entity : _scene->getEntities())
  {
    initEntityShader(entity);
  }

#ifdef DEBUG
  for (const Entity& entity : _scene->getDebugEntities())
  {
    initEntityShader(entity);
  }
#endif

  initWorldShader();
}

void ShaderManager::initEntityShader(const Entity& entity)
{
  Shader* s = (Shader*)entity.getComponent(BitMasks::Components::SHADER);
  assert(s != nullptr);
  if (s->initialized)
  {
    return;
  }

#ifdef DEBUG
  logDebug(std::format("{} ### Initializing shader for Entity: {}.\n", getName(), entity.getId()));
#endif

  GLuint programId = -1;
  if (_programList.find(s->shadergroup) != _programList.end())
  {
#ifdef DEBUG
    std::uint8_t cycle = getLogger()->getCycleLength();
    if (cycle == 0)
    {
      logCycle(std::format("{} ### Shader already created setting programId: {} "
                           "for group: {}.\n",
                           getName(),
                           _programList[s->shadergroup],
                           s->shadergroup));
    }
#endif
    programId = _programList[s->shadergroup];
  }
  else
  {
    programId = initShader(s, entity);
  }

  if (programId == -1)
  {
    logFailure(std::format("{} ### Could not create Shader for entity: {}.\n", getName(), entity.getId()));
    return;
  }

  s->programId   = programId;
  s->initialized = true;
  addUniforms(programId, entity);
}

void ShaderManager::initWorldShader()
{
  World* world = _scene->getWorld();
  if (world == nullptr)
  {
    return;
  }

  Shader* ws = (Shader*)world->getComponent(BitMasks::Components::SHADER);
  if (ws == nullptr || ws->initialized)
  {
    return;
  }

#ifdef DEBUG
  logDebug(std::format("{} ### Initializing world shader for group: {}.\n", getName(), ws->shadergroup));
#endif

  GLuint programId = -1;
  if (_programList.find(ws->shadergroup) != _programList.end())
  {
    programId = _programList[ws->shadergroup];
  }
  else
  {
    programId = initShader(ws, *world);
  }

  if (programId == -1)
  {
    logFailure(std::format("{} ### Could not create world shader.\n", getName()));
    return;
  }

  ws->programId   = programId;
  ws->initialized = true;
#ifdef DEBUG
  logDebug(std::format("{} ### World shader initialized. ProgramID: {}.\n", getName(), programId));
#endif

  Material* mat = (Material*)world->getComponent(BitMasks::Components::MATERIAL);
  if (mat != nullptr)
  {
    mat->diffuseColorUniform = UniformUtils::createUniform(UniformConstants::DCOLOR, programId);
#ifdef DEBUG
    logDebug(std::format("{} ### World color uniform location: {}.\n", getName(), mat->diffuseColorUniform));
#endif
  }
  world->setFlagUniform(UniformUtils::createUniform(UniformConstants::WORLD_TILEARRAY, programId));
  world->setGridUniform(UniformUtils::createUniform(UniformConstants::WORLD_GRIDSIZE, programId));
}

GLuint ShaderManager::initInterfaceShader()
{
  GLuint programId = glCreateProgram();
  if (programId == -1)
  {
    logFailure(std::format("{} ### Failed to create interface shader program.\n", getName()));
    return 1;
  }
  GLuint vertId = createVertexShader(std::string(WestInterfaceFacade::interfaceVertexShader), programId);
  GLuint fragId = createFragmentShader(std::string(WestInterfaceFacade::interfaceFragementShader), programId);
  if (vertId == -1 || fragId == -1)
  {
    logFailure(std::format("{} ### Failed to create vertex/fragement interface shader.\n", getName()));
    return 1;
  }
  link(programId, vertId, fragId);

#ifdef DEBUG
  logDebug(std::format("{} ### Created Shader for Interfaces. ProgramID: {}.\n", getName(), programId));
#endif

  Config::interfaceShaderProgram = programId;
  Config::interfaceOrthoUniform  = UniformUtils::createUniform(UniformConstants::ORTHO_UNIFORM, programId);
  if (Config::interfaceOrthoUniform == -1)
  {
    logFailure(std::format("{} ### Failed to create ortho matrix uniform for "
                           "interface shader program.\n",
                           getName()));
    return 1;
  }
  Config::interfaceFontTextureUniform = UniformUtils::createUniform(UniformConstants::FONT_TEXTURE_SAMPLER, programId);
  if (Config::interfaceFontTextureUniform == -1)
  {
    logFailure(std::format("{} ### Failed to create font sampler uniform for "
                           "interface shader program.\n",
                           getName()));
    return 1;
  }
  Config::interfaceTextureOneUniform = UniformUtils::createUniform(UniformConstants::TEXTURE_SAMPLER, programId);
  if (Config::interfaceTextureOneUniform == -1)
  {
    logFailure(std::format("{} ### Failed to create texture one uniform for "
                           "interface shader program.\n",
                           getName()));
    return 1;
  }
  return 0;
}

GLuint ShaderManager::initShader(Shader* s, const Entity& entity)
{
#ifdef DEBUG
  logDebug(std::format("{} ### Creating new Shader for group: {}.\n", getName(), s->shadergroup));
#endif
  GLuint programId = glCreateProgram();
  if (programId == 0)
  {
    return -1;
  }

#ifdef DEBUG
  logDebug(std::format("{} ### Created Shader for group: {}. ProgramID: {}.\n", getName(), s->shadergroup, programId));
#endif

  GLuint vertId = createVertexShader(s->vertexShaderFile, programId);
  GLuint fragId = createFragmentShader(s->fragShaderFile, programId);

  link(programId, vertId, fragId);
  _programList[s->shadergroup] = programId;
  GLuint uniformBlockIndex     = glGetUniformBlockIndex(programId, UniformConstants::CAMERA_UNIFORMS);

#ifdef DEBUG
  logDebug(std::format("{} ### Linked program: {}.\n", getName(), programId));
#endif

  if (uniformBlockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programId, uniformBlockIndex, 1);
  }
  else
  {
    logDebug(std::format("{} ### Uniform Block not found for Enitity: {}. Check if "
                         "the Entity uses a Shader with Camera uniforms\n",
                         getName(),
                         entity.getId()));
  }
  return programId;
}

// TODO: make switch case, also uniform locations queried per entity even when sharing the same shader program - cache
// per program
void ShaderManager::addUniforms(GLuint programId, const Entity& entity)
{
  Model* m     = (Model*)entity.getComponent(BitMasks::Components::MODEL);
  Material* m2 = (Material*)entity.getComponent(BitMasks::Components::MATERIAL);
  if (m2 != nullptr && m2->diffuseTexture != nullptr)
  {
    m2->diffuseTexture->uniform = UniformUtils::createUniform(UniformConstants::TEXTURE_SAMPLER, programId);
  }

  if (m != nullptr && !entity.isDebugEntity() && m2 != nullptr)
  {
    m2->diffuseColorUniform = UniformUtils::createUniform(UniformConstants::COLOR, programId);
  }

#ifdef DEBUG
  if (m != nullptr && entity.isDebugEntity())
  {
    m->debugColorUniform = UniformUtils::createUniform(UniformConstants::DCOLOR, programId);
  }
#endif

  Position* pos = (Position*)entity.getComponent(BitMasks::Components::POSITION);
  if (pos != nullptr)
  {
    pos->uniform = UniformUtils::createUniform(UniformConstants::TRANSFORMATION_MATRIX, programId);
  }
}

GLuint ShaderManager::createShader(const std::string shaderFile, std::int32_t shaderType, GLuint programId)
{
  GLuint shaderId = glCreateShader(shaderType);
  if (shaderId == 0)
  {
    logFailure(std::format("Error creating shader. Type: {}.\n", shaderType));
    return -1;
  }

#ifdef DEBUG
  logDebug(std::format("{} ### Created Shader: {} for File: {}.\n", getName(), shaderId, shaderFile));
#endif

  const GLchar* source = readShaderSource(shaderFile);
  if (source == NULL)
  {
    logFailure("Failed to read shader source from file\n");
    return -1;
  }
  GLchar errorLog[2048] = {};
  GLint size = 0, status = 0;
  glShaderSource(shaderId, 1, &source, NULL);
  glCompileShader(shaderId);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == 0)
  {
    glGetShaderInfoLog(shaderId, 2048, &size, errorLog);
    logFailure(std::format("Error compiling shader. Type: {}, Info: {}\n", shaderType, errorLog));
    return -1;
  }

  glAttachShader(programId, shaderId);
  return shaderId;
}

void ShaderManager::link(GLuint programId, GLuint vertexId, GLuint fragmentId)
{
  glLinkProgram(programId);
  GLint size = 0, status = 0;
  GLchar errorLog[1024] = {};
  glGetProgramiv(programId, GL_LINK_STATUS, &status);
  if (status == 0)
  {
    glGetProgramInfoLog(programId, 1024, &size, errorLog);
    logFailure(std::format("Error linking program. Info: {}\n", errorLog));
  }

  if (vertexId != 0)
  {
    glDetachShader(programId, vertexId);
  }

  if (fragmentId != 0)
  {
    glDetachShader(programId, fragmentId);
  }

  glValidateProgram(programId);
  glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
  if (status == 0)
  {
    logFailure(std::format("Error validating program. Info: {}\n", errorLog));
  }
}

GLchar* ShaderManager::readShaderSource(const std::string shaderFile)
{
  FILE* file = PathUtils::openFile(shaderFile, true);
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);
  char* source = (char*)malloc((fileSize + 1) * sizeof(char));
  if (source == NULL)
  {
    logFailure("Error allocating memory.\n");
    fclose(file);
    return nullptr;
  }

  size_t bytesRead = fread(source, sizeof(char), fileSize, file);
  if (bytesRead != fileSize)
  {
    logFailure("Could not read the Entire file.\n");
    fclose(file);
    free(source);
    return nullptr;
  }

  source[fileSize] = '\0';
  fclose(file);
  return source;
}
