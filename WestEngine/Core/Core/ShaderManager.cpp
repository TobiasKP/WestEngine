#include "../CoreHeaders/ShaderManager.h"

#include "../Config/Config.h"
#include "../Constants/UniformConstants.h"
#include "../CoreHeaders/Utils/DataUtils/UniformUtils.h"
#include "../CoreHeaders/Utils/TimeUtils.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

ShaderManager::ShaderManager() : IManager(nullptr) {
  setName(CoreConstants::SHADER_MANAGER);
}

ShaderManager::ShaderManager(WestLogger *logger) : IManager(logger) {
  setName(CoreConstants::SHADER_MANAGER);
}

ShaderManager::~ShaderManager() {}

std::int32_t ShaderManager::startup() { return 0; }

void ShaderManager::shutdown() {
#ifdef DEBUG
  getString()->format("%s ### Shutting down %s...\n", getName(), getName());
  logDebug(getString()->getBuffer());
#endif

  glUseProgram(0);
  for (auto &entity : _scene->getEntities()) {
    Shader *s = (Shader *)entity->getComponent(BitMasks::Components::SHADER);
    GLuint programId = s->programId;
    glDeleteProgram(programId);
  }
}

std::int32_t ShaderManager::init() {
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif

  _scene = &Scene::getSceneInstance();
  _scene->getCamera()->setCameraUniforms(
      UniformUtils::createUniformBufferObject(UniformConstants::CAMERA_UNIFORMS,
                                              sizeof(glm::mat4) * 2, 1));
  //initInterfaceShader();

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  double res = TimeUtils::getDuration(start, end);
  getString()->format("%s ### ShaderManager init time: %f ms.\n", getName(),
                      res);
  logDebug(getString()->getBuffer());
#endif

  return 0;
}

void ShaderManager::update() {
  for (auto &entity : _scene->getEntities()) {
    Shader *s = (Shader *)entity->getComponent(BitMasks::Components::SHADER);
    if (s->initialized)
      continue;

#ifdef DEBUG
    getString()->format("%s ### Initializing shader for Entity: %d.\n",
                        getName(), entity->getId());
    logDebug(getString()->getBuffer());
#endif

    GLuint programId = -1;
    if (_programList.find(s->shadergroup) != _programList.end()) {
#ifdef DEBUG
      getString()->format("%s ### Shader already created setting programId: %d "
                          "for group: %d.\n",
                          getName(), _programList[s->shadergroup],
                          s->shadergroup);
      logDebug(getString()->getBuffer());
#endif
      programId = _programList[s->shadergroup];
    } else {
      programId = initShader(s, entity);
    }

    if (programId == -1) {
      getString()->format("%s ### Could not create Shader for entity: %d.\n",
                          getName(), entity->getId());
      logFailure(getString()->getBuffer());
    }

    s->programId = programId;
    s->initialized = true;
    addUniforms(programId, entity, s->shadergroup);
  }
}

void ShaderManager::initInterfaceShader() {
  GLuint programId = glCreateProgram();
  assert(Config::Interface.VERTEX_LOCATION != nullptr &&
         Config::Interface.FRAG_LOCATION != nullptr);
  GLuint vertId =
      createVertexShader(Config::Interface.VERTEX_LOCATION, programId);
  GLuint fragId =
      createFragmentShader(Config::Interface.FRAG_LOCATION, programId);
  link(programId, vertId, fragId);

  if (programId == -1) {
    getString()->format("%s ### Failed to create interface shader program.\n",
                        getName());
    logFailure(getString()->getBuffer());
  }
#ifdef DEBUG
  getString()->format("%s ### Created Shader for Interfaces. ProgramID: %d.\n",
                      getName(), programId);
  logDebug(getString()->getBuffer());
#endif

  _programList[CoreConstants::TEXT_SHADERGROUP] = programId;
  // Global::UserInterface::SHADER_PROGRAM = programId;
  // Global::UserInterface::ORTHO_UNIFORM =
  //     UniformUtils::createUniform(UniformConstants::ORTHO_UNIFORM,
  //     programId);
  // Global::UserInterface::TEXTURE_SAMPLER =
  //     UniformUtils::createUniform(UniformConstants::TEXTURE_SAMPLER,
  //     programId);
}

GLuint ShaderManager::initShader(Shader *s, Entity *entity) {
#ifdef DEBUG
  getString()->format("%s ### Creating new Shader for group: %d.\n", getName(),
                      s->shadergroup);
  logDebug(getString()->getBuffer());
#endif
  GLuint programId = glCreateProgram();
  if (programId == 0)
    return -1;

#ifdef DEBUG
  getString()->format("%s ### Created Shader for group: %d. ProgramID: %d.\n",
                      getName(), s->shadergroup, programId);
  logDebug(getString()->getBuffer());
#endif

  GLuint vertId = createVertexShader(s->vertexShaderFile, programId);
  GLuint fragId = createFragmentShader(s->fragShaderFile, programId);
  link(programId, vertId, fragId);
  _programList[s->shadergroup] = programId;
  GLuint uniformBlockIndex =
      glGetUniformBlockIndex(programId, UniformConstants::CAMERA_UNIFORMS);
  if (uniformBlockIndex != GL_INVALID_INDEX)
    glUniformBlockBinding(programId, uniformBlockIndex, 1);
  else {
    getString()->format(
        "%s ### Uniform Block not found for Enitity: %d. Check if the "
        "Entity uses a Shader with Camera uniforms\n",
        getName(), entity->getId());
    logDebug(getString()->getBuffer());
  }
  return programId;
}

// TODO make switch case
void ShaderManager::addUniforms(GLuint programId, Entity *entity,
                                std::int32_t shadergroup) {
  Model *m = (Model *)entity->getComponent(BitMasks::Components::MODEL);
  if (m != nullptr && m->texture != nullptr)
    m->texture->uniform = UniformUtils::createUniform(
        UniformConstants::TEXTURE_SAMPLER, programId);

#ifdef DEBUG
  if (m != nullptr && entity->isDebugEntity())
    m->debugColorUniform =
        UniformUtils::createUniform(UniformConstants::COLOR, programId);
#endif

  Position *pos =
      (Position *)entity->getComponent(BitMasks::Components::POSITION);
  if (pos != nullptr) {
    pos->uniform = UniformUtils::createUniform(
        UniformConstants::TRANSFORMATION_MATRIX, programId);
  }
}

GLuint ShaderManager::createShader(const char *shaderFile,
                                   std::int32_t shaderType, GLuint programId) {
  GLuint shaderId = glCreateShader(shaderType);
  if (shaderId == 0) {
    getString()->format("Error creating shader. Type: %d.\n", shaderType);
    logFailure(getString()->getBuffer());
    return -1;
  }

#ifdef DEBUG
  getString()->format("%s ### Created Shader: %d from File: %s.\n", getName(),
                      shaderId, shaderFile);
  logDebug(getString()->getBuffer());
#endif

  const GLchar *source = readShaderSource(shaderFile);
  if (source == NULL) {
    logFailure("Failed to read shader source from file\n");
    return 0;
  }
  GLchar errorLog[2048] = {};
  GLint size = 0, status = 0;
  glShaderSource(shaderId, 1, &source, NULL);
  glCompileShader(shaderId);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == 0) {
    glGetShaderInfoLog(shaderId, 2048, &size, errorLog);
    getString()->format("Error compiling shader. Type: %d, Info: %s\n",
                        shaderType, errorLog);
    logFailure(getString()->getBuffer());
    return 0;
  }

  glAttachShader(programId, shaderId);
  return shaderId;
}

void ShaderManager::link(GLuint programId, GLuint vertexId, GLuint fragmentId) {
  glLinkProgram(programId);
  GLint size = 0, status = 0;
  GLchar errorLog[1024] = {};
  glGetProgramiv(programId, GL_LINK_STATUS, &status);
  if (status == 0) {
    glGetProgramInfoLog(programId, 1024, &size, errorLog);
    getString()->format("Error linking program. Info: %s\n", errorLog);
    logFailure(getString()->getBuffer());
  }

  if (vertexId != 0)
    glDetachShader(programId, vertexId);

  if (fragmentId != 0)
    glDetachShader(programId, fragmentId);

  glValidateProgram(programId);
  glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
  if (status == 0) {
    getString()->format("Error validating program. Info: %s\n", errorLog);
    logFailure(getString()->getBuffer());
  }
}

GLchar *ShaderManager::readShaderSource(const char *shaderFile) {
  std::int32_t fd;
  if (open(shaderFile, O_RDONLY) == -1) {
    getString()->format("Error opening shader File. Path: %s\n", shaderFile);
    logFailure(getString()->getBuffer());
    return nullptr;
  }

  FILE *file = fdopen(fd, "rb");
  if (file == NULL) {
    getString()->format("Error opening File. Path: %s\n", shaderFile);
    logFailure(getString()->getBuffer());
    return nullptr;
  }

  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);
  char *source = (char *)malloc((fileSize + 1) * sizeof(char));
  if (source == NULL) {
    logFailure("Error allocating memory.\n");
    fclose(file);
    return nullptr;
  }

  size_t bytesRead = fread(source, sizeof(char), fileSize, file);
  if (bytesRead != fileSize) {
    logFailure("Could not read the Entire file.\n");
    fclose(file);
    free(source);
    return nullptr;
  }

  source[fileSize] = '\0';
  fclose(file);
  return source;
}
