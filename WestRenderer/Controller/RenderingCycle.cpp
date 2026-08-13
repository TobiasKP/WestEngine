#include "RenderingCycle.hpp"

#include "../UniformConstants.hpp"

#include <glm/ext/matrix_clip_space.hpp>
#include <TimeUtils.hpp>
#include <WestInterfaceFacade.h>

void RenderingCycle::renderEntity(std::string guid,
                                  GLuint programId,
                                  std::unordered_map<std::string, GLuint>& uniforms,
                                  EntityUniformParams* params,
                                  bool debug)
{
#ifdef DEBUG
  double start = TimeUtils::getCurrentTimeAsTime();
#endif
  if (programId != _lastUsedShader)
  {
    glUseProgram(programId);
    _lastUsedShader = programId;
  }

#ifdef DEBUG
  GLint linked;
  glGetProgramiv(programId, GL_LINK_STATUS, &linked);
  assert(linked == GL_TRUE);
  double step = TimeUtils::getCurrentTimeAsTime();
  double res  = TimeUtils::getDuration(start, step);
  _logger->log(Level::Cycle, std::format("|><| Switching shader and asserting linkage: {} ms.\n", res));
#endif


  const Model* model = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(guid);
  if (model == nullptr)
  {
    _logger->log(Level::Error, std::format("|><| Can not retrieve model for guid: {}\n", guid));
    glUseProgram(0);
    return;
  }

#ifdef DEBUG
  step = TimeUtils::getCurrentTimeAsTime();
  res  = TimeUtils::getDuration(start, step);
  _logger->log(Level::Cycle, std::format("|><| Requesting Model for rendering: {} ms.\n", res));
#endif

  if (!_uuidToVAO.contains(guid))
  {
    registerModel(model, guid);
  }

#ifdef DEBUG
  step = TimeUtils::getCurrentTimeAsTime();
  res  = TimeUtils::getDuration(start, step);
  _logger->log(Level::Cycle, std::format("|><| Registering Model for OpenGL: {} ms.\n", res));
#endif
  const std::vector<std::pair<std::string, std::uint32_t>>& vaos = _uuidToVAO.at(guid);
  for (std::uint32_t vao = 0; vao < vaos.size(); vao++)
  {
#ifdef DEBUG
    double singleEntity = TimeUtils::getCurrentTimeAsTime();
#endif
    const std::string& meshGuid = std::get<0>(vaos[vao]);
    const std::uint32_t v       = std::get<1>(vaos[vao]);
    if (uniforms.contains(UniformConstants::COLOR))
    {
      _utils->setUniform(uniforms[UniformConstants::COLOR], params->diffuseColor[meshGuid]);
    }

#ifdef DEBUG
    if (debug && uniforms.contains(UniformConstants::DCOLOR))
    {
      _utils->setUniform(uniforms[UniformConstants::DCOLOR], params->diffuseColor[meshGuid]);
    }
#endif

    if (uniforms.contains(UniformConstants::ECOLOR))
    {
      _utils->setUniform(uniforms[UniformConstants::ECOLOR], params->emissiveColor[meshGuid]);
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
    step = TimeUtils::getCurrentTimeAsTime();
    res  = TimeUtils::getDuration(singleEntity, step);
    _logger->log(Level::Cycle, std::format("|><| Setting Uniforms for: {} in: {} ms.\n", guid, res));
    _logger->log(Level::Cycle, std::format("|><| guid {} has {} vaos\n", guid, _uuidToVAO[guid].size()));
#endif

    glBindVertexArray(_vaos.at(v));

#ifdef DEBUG
    if (debug)
    {
      glDisable(GL_DEPTH_TEST);
      glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);
      glEnable(GL_DEPTH_TEST);
    }
    else
    {
      glDrawElements(GL_TRIANGLES, _uuidToVertexCount.at(guid)[vao], GL_UNSIGNED_INT, 0);
    }

    step = TimeUtils::getCurrentTimeAsTime();
    res  = TimeUtils::getDuration(singleEntity, step);
    _logger->log(Level::Cycle, std::format("|><| Rendering for: {} took: {} ms.\n", guid, res));
#else
    glDrawElements(GL_TRIANGLES, _uuidToVertexCount.at(guid)[vao], GL_UNSIGNED_INT, 0);
#endif
  }

#ifdef DEBUG
  double end = TimeUtils::getCurrentTimeAsTime();
  res        = TimeUtils::getDuration(start, end);
  _logger->log(Level::Cycle, std::format("|><| Render cycle complete time: {} ms.\n", res));
#endif
};

void RenderingCycle::renderWorld(std::unordered_map<std::string, GLuint> uniforms,
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
  if (!_uuidToVAO.contains(params->skyboxGuid))
  {
    const Model* skybox = WestData::WestAssetFacade::getAssetFacade().requestModelFromScene(params->skyboxGuid);
    if (skybox == nullptr)
    {
      _logger->log(Level::Error, std::format("|><| Can not retrieve World model for guid: {}\n", modelGuid));
      glUseProgram(0);
      return;
    }
    registerModel(skybox, params->skyboxGuid);
    std::vector<Texture> textures;
    for (const Mesh m : skybox->getMeshes())
    {
      textures.insert(textures.end(), m.textures.begin(), m.textures.end());
    }
    loadCubemap(params->skyboxGuid, textures);
  }

  if (dirty)
  {
    _utils->setUniform(uniforms[UniformConstants::WORLD_TILEARRAY], params->flags);
    _utils->setUniform(uniforms[UniformConstants::WORLD_GRIDSIZE], params->worldDimension);
    _utils->setUniform(uniforms[UniformConstants::WORLD_GRID_ORIGIN], params->worldOrigin);
  }

  for (std::uint32_t i = 0; i < _uuidToVAO[modelGuid].size(); i++)
  {
    if (uniforms.contains(UniformConstants::DCOLOR))
    {
      _utils->setUniform(uniforms[UniformConstants::DCOLOR], model->getMeshes().front().material.diffuseColor);
    }
    std::uint32_t idx = std::get<1>(_uuidToVAO[modelGuid][i]);
    glBindVertexArray(_vaos[idx]);
    glDrawElements(GL_TRIANGLES, _uuidToVertexCount[modelGuid][i], GL_UNSIGNED_INT, 0);
  }

  renderSkybox(uniforms, params);
  glUseProgram(_lastUsedShader);
};

void RenderingCycle::renderSkybox(const std::unordered_map<std::string, GLuint>& uniforms, WorldUniformParams* params)
{
  if (!_uuidToVAO.contains(params->skyboxGuid) || !_uuidToTexture.contains(params->skyboxGuid))
  {
    return;
  }
 
  glUseProgram(params->skyboxShaderId);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_FALSE);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, _uuidToTexture.at(params->skyboxGuid));
  if (uniforms.contains(UniformConstants::SKYBOX_CUBE_TEX))
  {
    _utils->setUniform(uniforms.at(UniformConstants::SKYBOX_CUBE_TEX), 0);
  }

  const std::vector<std::pair<std::string, std::uint32_t>>& vaos = _uuidToVAO.at(params->skyboxGuid);
  const std::vector<std::uint32_t>& counts                       = _uuidToVertexCount.at(params->skyboxGuid);
  for (std::uint32_t i = 0; i < vaos.size(); i++)
  {
    glBindVertexArray(_vaos[std::get<1>(vaos[i])]);
    glDrawElements(GL_TRIANGLES, counts[i], GL_UNSIGNED_INT, 0);
  }

  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
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
  size_t vboStart = _vbos.size();
  for (size_t i = 0; i < model->getMeshes().size(); i++)
  {
    const Mesh& m = model->getMeshes()[i];
    createVAO();
    _uuidToVAO[modelGuid].push_back({m.getGuid(), _vaos.size() - 1});
    storeIndicesBuffer(m.indices.data(), m.indices.size());
    _uuidToVertexCount[modelGuid].push_back(m.indices.size());
    std::vector<float> positions;
    positions.reserve(m.vertices.size() * 3);
    std::vector<float> texCoords;
    texCoords.reserve(m.vertices.size() * 2);
    std::vector<float> normals;
    normals.reserve(m.vertices.size() * 3);
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
  _uuidToVBOs[modelGuid] = std::vector<GLuint>(_vbos.begin() + vboStart, _vbos.end());
}

void RenderingCycle::cleanupModel(const std::string& modelGuid)
{
  auto vaoIt = _uuidToVAO.find(modelGuid);
  if (vaoIt == _uuidToVAO.end())
  {
    return;
  }

  for (std::uint32_t i = 0; i < vaoIt->second.size(); i++)
  {
    std::uint32_t idx = std::get<1>(vaoIt->second[i]);
    GLuint vao        = _vaos[idx];
    glDeleteVertexArrays(1, &vao);
  }


  auto vboIt = _uuidToVBOs.find(modelGuid);
  if (vboIt != _uuidToVBOs.end())
  {
    glDeleteBuffers(vboIt->second.size(), vboIt->second.data());
    _uuidToVBOs.erase(vboIt);
  }

  _uuidToVAO.erase(vaoIt);
  _uuidToVertexCount.erase(modelGuid);
  _uuidToTexture.erase(modelGuid);
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

void RenderingCycle::loadCubemap(std::string& guid, std::vector<Texture> faces)
{
  if (faces.size() != 6)
  {
    _logger->log(Level::Error,
                 std::format("|><| Cubemap for guid {} needs 6 faces, got {}. Skybox will sample black.\n",
                             guid,
                             faces.size()));
  }

  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
  std::uint32_t i = 0;
  for (Texture& t : faces)
  {
    const GLenum format = t.numComponents == 4 ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                 0,
                 format,
                 t.width,
                 t.height,
                 0,
                 format,
                 GL_UNSIGNED_BYTE,
                 t.imageData.data());
    i++;
  }
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  _uuidToTexture[guid] = textureId;
};
