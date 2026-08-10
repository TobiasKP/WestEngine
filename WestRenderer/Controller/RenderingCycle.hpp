#pragma once

#include "../UniformParams.hpp"
#include "../Uniforms/UniformUtils.hpp"

#include <GL/glew.h>
#include <unordered_map>
#include <WestAssetFacade.hpp>
#include <WestLogger.h>

class RenderingCycle
{
public:
  RenderingCycle(WestLogger* l, std::shared_ptr<UniformUtils> utils) : _logger(l), _utils(utils) {}
  ~RenderingCycle() {};

  void renderEntity(std::string guid,
                    GLuint programId,
                    std::unordered_map<std::string, GLuint>& uniforms,
                    EntityUniformParams* params,
                    bool debug);
  void renderWorld(std::unordered_map<std::string, GLuint> uniforms,
                   GLuint programId,
                   std::string modelGuid,
                   bool dirty,
                   WorldUniformParams* params);
  void renderInterfaces();
  void cleanupModel(const std::string& modelGuid);

private:
  void registerModel(const Model* model, std::string modelGuid);
  void createVAO();
  void storeIndicesBuffer(const std::uint32_t* data, size_t dataLength);
  void storeDataInAttribList(std::int32_t attribNo, std::int32_t vertexCount, const float* data, size_t dataLength);

  std::shared_ptr<UniformUtils> _utils;
  WestLogger* _logger;
  WestData::WestAssetFacade* _dataFacade;
  GLuint _lastUsedShader = 0;
  std::unordered_map<std::string, std::vector<std::pair<std::string, std::uint32_t>>> _uuidToVAO;
  std::unordered_map<std::string, std::vector<std::uint32_t>> _uuidToVertexCount;
  std::unordered_map<std::string, GLint> _uuidToTexture;
  std::unordered_map<std::string, std::vector<GLuint>> _uuidToVBOs;
  std::vector<GLuint> _vaos;
  std::vector<GLuint> _vbos;
};
