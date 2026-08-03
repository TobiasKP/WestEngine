#include "../WestRendererFacade.hpp"

#include "../UniformConstants.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string.h>

using namespace WestRenderer;

WestRendererFacade& WestRendererFacade::getRendererFacade()
{
  static WestRendererFacade instance;
  return instance;
};


WestRendererFacade::WestRendererFacade()
{
  _logger     = &WestLogger::getLoggerInstance();
  _controller = std::make_unique<ShaderController>(_logger);
  _utils      = std::make_shared<UniformUtils>();
  _cycle      = std::make_unique<RenderingCycle>(_logger, _utils);
}

WestRendererFacade::~WestRendererFacade() {}

void WestRendererFacade::clearColor()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
};

void WestRendererFacade::renderEntity(
  std::string guid, std::uint32_t entityId, GLuint programId, EntityUniformParams* params, bool debug)
{
  std::unordered_map<const char*, GLuint> uniforms = _entityToUniforms[entityId];
  _logger->log(Level::Info, std::format("|><| renderEntity entityId={} programId={} uniforms.size={}\n", entityId, programId, uniforms.size()));
  _cycle->renderEntity(guid, programId, uniforms, params, debug);
};

void WestRendererFacade::renderInterface()
{
  _cycle->renderInterfaces();
};

void WestRendererFacade::renderWorld(
  std::string modelGuid, std::uint32_t entityId, GLuint programId, bool dirty, WorldUniformParams* params)
{
  std::unordered_map<const char*, GLuint> uniforms = _entityToUniforms[entityId];
  _cycle->renderWorld(uniforms, programId, modelGuid, dirty, params);
};

void WestRendererFacade::renderDebugEntities() {};

void WestRendererFacade::updateCamera(std::uint32_t entityId, glm::mat4 view, glm::mat4 projection)
{
  const char* key                                  = UniformConstants::CAMERA_UNIFORMS;
  std::unordered_map<const char*, GLuint> uniforms = _entityToUniforms[entityId];
  _logger->log(Level::Info, std::format("|><| updateCamera entityId={} uniforms.size={}\n", entityId, uniforms.size()));
  if (uniforms.contains(key))
  {
    glBindBuffer(GL_UNIFORM_BUFFER, uniforms[key]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }
};

GLuint WestRendererFacade::registerShader(const std::string& vshader, const std::string& fshader)
{
  auto findV = _vshaderToProgramId.find(vshader);
  auto findF = _fshaderToProgramId.find(fshader);
  if (findV != _vshaderToProgramId.end() && findF != _fshaderToProgramId.end() && findV->second == findF->second)
  {
    return findV->second;
  }

#ifdef DEBUG
  _logger->log(Level::Info, std::format("|><| ### Creating new Shader.\n"));
#endif
  GLuint programId = glCreateProgram();
  if (programId == 0)
  {
    return -1;
  }

#ifdef DEBUG
  _logger->log(Level::Info, std::format("|><| ### Created Shader for ProgramID: {}.\n", programId));
#endif

  GLuint vertId = _controller->createVertexShader(vshader, programId);
  GLuint fragId = _controller->createFragmentShader(fshader, programId);

  _controller->link(programId, vertId, fragId);
  GLuint uniformBlockIndex = glGetUniformBlockIndex(programId, UniformConstants::CAMERA_UNIFORMS);

#ifdef DEBUG
  _logger->log(Level::Info, std::format("|><| ### Linked program: {}.\n", programId));
#endif

  if (uniformBlockIndex != GL_INVALID_INDEX)
  {
    glUniformBlockBinding(programId, uniformBlockIndex, 1);
  }
  else
  {
    _logger->log(Level::Info,
                 std::format("|><| ### Uniform Block not found for shader: {} and {}. Check if "
                             "the Entity uses a Shader with Camera uniforms\n",
                             vshader,
                             fshader));
  }
  _vshaderToProgramId[vshader] = programId;
  _fshaderToProgramId[vshader] = programId;
  return programId;
};

void WestRendererFacade::createUniform(const char* name, GLuint programId, std::uint32_t entityId)
{
  GLuint location = _utils->createUniform(name, programId);
  if (location == -1)
  {
    return;
  }

  _entityToUniforms[entityId][name] = location;
}

void WestRendererFacade::createUniformBufferObject(const char* name,
                                                   size_t size,
                                                   GLuint bindingPoint,
                                                   std::uint32_t entityId)
{
  GLuint location = _utils->createUniformBufferObject(name, size, bindingPoint);
  if (location == -1)
  {
    return;
  }
  std::pair nameAndLocation{name, location};
  _entityToUniforms[entityId][name] = location;
}
