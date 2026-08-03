#include "RenderingCycle.hpp"

#include "../UniformConstants.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <WestInterfaceFacade.h>

void RenderingCycle::renderEntity(std::string guid,
                                  GLuint programId,
                                  std::unordered_map<const char*, GLuint> uniforms,
                                  EntityUniformParams* params,
                                  bool debug)
{
  if (programId != _lastUsedShader)
  {
    glUseProgram(programId);
    _lastUsedShader = programId;
  }

#ifdef DEBUG
  GLint linked;
  glGetProgramiv(programId, GL_LINK_STATUS, &linked);
  assert(linked == GL_TRUE);
#endif


  const Model* model = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(guid);
  if (model == nullptr)
  {
    _logger->log(Level::Error, std::format("|><| Can not retrieve model for guid: {}\n", guid));
    glUseProgram(0);
    return;
  }


  if (!_uuidToVAO.contains(guid))
  {
    registerModel(model, guid);
  }

  if (uniforms.contains(UniformConstants::COLOR))
  {
    _utils->setUniform(uniforms[UniformConstants::COLOR], model->getMeshes().front().material.diffuseColor);
  }

  if (uniforms.contains(UniformConstants::ECOLOR))
  {
    _utils->setUniform(uniforms[UniformConstants::ECOLOR], model->getMeshes().front().material.emissiveColor);
  }

  if (uniforms.contains(UniformConstants::TEXTURE_SAMPLER))
  {
    _utils->setUniform(uniforms[UniformConstants::TEXTURE_SAMPLER], _uuidToTexture[guid]);
  }

  if (uniforms.contains(UniformConstants::TRANSFORMATION_MATRIX))
  {
    _utils->setUniform(uniforms[UniformConstants::TRANSFORMATION_MATRIX], params->transform);
  }
 
#ifdef DEBUG
  _logger->log(Level::Cycle, std::format("|><| guid {} has {} vao", guid, _uuidToVAO[guid]));
#endif

  glBindVertexArray(_vaos[_uuidToVAO[guid]]);

#ifdef DEBUG
  if (debug)
  {
    glDisable(GL_DEPTH_TEST);
    glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);
  }
  else
  {
    glDrawElements(GL_TRIANGLES, _uuidToVertexCount[guid], GL_UNSIGNED_INT, 0);
  }
#else
  glDrawElements(GL_TRIANGLES, _uuidToVertexCount[guid], GL_UNSIGNED_INT, 0);
#endif
};

void RenderingCycle::renderWorld(std::unordered_map<const char*, GLuint> uniforms,
                                 GLuint programId,
                                 std::string modelGuid,
                                 bool dirty,
                                 WorldUniformParams* params)
{
  glEnable(GL_DEPTH_TEST);
  if (_lastUsedShader != programId)
  {
    glUseProgram(programId);
  }

  const Model* model = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(modelGuid);
  if (model == nullptr)
  {
    _logger->log(Level::Error, std::format("|><| Can not retrieve World model for guid: {}\n", modelGuid));
    glUseProgram(0);
    return;
  }

  if (!_uuidToVAO.contains(modelGuid))
  {
    registerModel(model, modelGuid);
  }

  if (dirty)
  {
    _utils->setUniform(uniforms[UniformConstants::WORLD_TILEARRAY], params->flags);
    _utils->setUniform(uniforms[UniformConstants::WORLD_GRIDSIZE], params->worldDimension);
    _utils->setUniform(uniforms[UniformConstants::WORLD_GRID_ORIGIN], params->worldOrigin);
  }


  glBindVertexArray(_vaos[_uuidToVAO[modelGuid]]);
  glDrawElements(GL_TRIANGLES, _uuidToVertexCount[modelGuid], GL_UNSIGNED_INT, 0);
  glUseProgram(_lastUsedShader);
};

void RenderingCycle::renderInterfaces()
{
  WestInterface::WestInterfaceFacade* facade = &WestInterface::WestInterfaceFacade::getInterfaceInstance();
  std::vector<ComponentData*> renderData     = facade->getRenderData();
  if (renderData.size() == 0)
  {
    _logger->log(Level::Error, "|><| No render Data for interfaces gathered skipping rendering!\n");
    return;
  }

  // TODO 4 vectors recreated per frame use GL_STREAM_DRAW or persistend mapped buffers
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
    _logger->log(Level::Error, "|><| No instance data for interfaces gathered skipping rendering.\n");
    return;
  }

#ifdef DEBUG
  std::uint8_t cycle = _logger->getCycleLength();
  if (cycle == 0)
  {
    _logger->log(Level::Cycle, "|><| Rendering interfaces ... \n");
  }
#endif

  glUseProgram(Config::interfaceShaderProgram);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBindVertexArray(facade->_interfaceVAO);

  glBindBuffer(GL_ARRAY_BUFFER, facade->_interfaceCOL);
  glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);


  glBindBuffer(GL_ARRAY_BUFFER, facade->_interfaceOFFSET);
  glBufferData(GL_ARRAY_BUFFER, instanceOffsets.size() * sizeof(float), instanceOffsets.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, facade->_interfaceFLAGS);
  glBufferData(GL_ARRAY_BUFFER, flags.size() * sizeof(std::uint32_t), flags.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, facade->_interfaceUV);
  glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(float), textCoords.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glActiveTexture(GL_TEXTURE0);
  _utils->setUniform(Config::interfaceFontTextureUniform, 0);
  glBindTexture(GL_TEXTURE_2D, facade->_interfaceFONT_TEXTURE_ID);

  glActiveTexture(GL_TEXTURE1);
  _utils->setUniform(Config::interfaceTextureOneUniform, 1);
  glBindTexture(GL_TEXTURE_2D, texture);

  glm::mat4 ortho = glm::ortho(0.0f, (float)Config::GeneralConfig.WIDTH, 0.0f, (float)Config::GeneralConfig.HEIGHT);
  _utils->setUniform(Config::interfaceOrthoUniform, ortho);
  glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, renderData.size());
  glDisable(GL_BLEND);
  glUseProgram(_lastUsedShader);
}


void RenderingCycle::registerModel(const Model* model, std::string modelGuid)
{
  createVAO();
  _uuidToVAO[modelGuid] = _vaos.size() - 1;
  for (const Mesh& m : model->getMeshes())
  {
    storeIndicesBuffer(m.indices.data(), m.indices.size());
    _uuidToVertexCount[modelGuid] = m.indices.size();
    std::vector<float> positions;
    positions.reserve(m.vertices.size() * 3);
    std::vector<float> texCoords;
    texCoords.reserve(m.vertices.size() * 2);
    std::vector<float> normals;
    positions.reserve(m.vertices.size() * 3);
    for (const Vertex& v : m.vertices)
    {
      positions.insert(positions.end(), {v.Position.x, v.Position.y, v.Position.z});
      texCoords.insert(texCoords.end(), {v.TexCoords.x, v.TexCoords.y});
      normals.insert(normals.end(), {v.Normal.x, v.Normal.y, v.Normal.z});
    }
    storeDataInAttribList(0, 3, positions.data(), positions.size());
    storeDataInAttribList(1, 2, texCoords.data(), texCoords.size());
    storeDataInAttribList(2, 3, normals.data(), normals.size());
  }
}

void RenderingCycle::createVAO()
{
  GLuint vao;
  glGenVertexArrays(1, &vao);
  _vaos.push_back(vao);
  glBindVertexArray(vao);
};

void RenderingCycle::storeIndicesBuffer(const std::uint32_t* data, size_t dataLength)
{
  assert(data != nullptr);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  _vbos.push_back(vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLength * sizeof(std::uint32_t), data, GL_STATIC_DRAW);
}

void RenderingCycle::storeDataInAttribList(std::int32_t attribNo,
                                           std::int32_t vertexCount,
                                           const float* data,
                                           size_t dataLength)
{
  assert(data != nullptr && attribNo >= 0 && vertexCount > 0);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  _vbos.push_back(vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, dataLength * sizeof(float), data, GL_STATIC_DRAW);
  glVertexAttribPointer(attribNo, vertexCount, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attribNo);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
