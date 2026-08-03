#pragma once

#include "Controller/RenderingCycle.hpp"
#include "Shader/ShaderController.hpp"
#include "UniformParams.hpp"
#include "Uniforms/UniformUtils.hpp"

#include <GL/glew.h>
#include <string>
#include <unordered_map>

namespace WestRenderer
{

class WestRendererFacade
{
public:
  static WestRendererFacade& getRendererFacade();

  void updateCamera(std::uint32_t entityId, glm::mat4 view, glm::mat4 projection);
  void clearColor();
  void renderEntity(
    std::string guid, std::uint32_t entityId, GLuint programId, EntityUniformParams* params, bool debug = false);
  void renderInterface();
  void
  renderWorld(std::string modelGuid, std::uint32_t entityId, GLuint programId, bool dirty, WorldUniformParams* params);
  void renderDebugEntities();
  void createUniform(const char* name, GLuint programId, std::uint32_t entityId);
  void createUniformBufferObject(const char* name, size_t size, GLuint bindingPoint, std::uint32_t entityId);

  GLuint registerShader(const std::string& vshader, const std::string& fshader);

private:
  WestRendererFacade();
  ~WestRendererFacade();
  WestRendererFacade(const WestRendererFacade& other)            = delete;
  WestRendererFacade& operator=(const WestRendererFacade& other) = delete;
  WestRendererFacade(WestRendererFacade&& other)                 = delete;
  WestRendererFacade& operator=(WestRendererFacade&& other)      = delete;

  std::unordered_map<std::string, GLuint> _vshaderToProgramId;
  std::unordered_map<std::string, GLuint> _fshaderToProgramId;
  std::unordered_map<std::uint32_t, std::unordered_map<std::string, GLuint>> _entityToUniforms;
  std::unique_ptr<ShaderController> _controller;
  std::shared_ptr<UniformUtils> _utils;
  std::unique_ptr<RenderingCycle> _cycle;
  WestLogger* _logger;
};

};  // namespace WestRenderer
