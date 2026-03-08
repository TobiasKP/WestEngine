#pragma once

#include "../../Components/Umbrella.h"

#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <WestLogger.h>

class ObjectLoader
{
public:
  ObjectLoader() : _logger(nullptr) {};
  ObjectLoader(WestLogger* logger) : _logger(logger) {};

  std::tuple<std::unique_ptr<Model>, std::unique_ptr<Material>, std::unique_ptr<AABB>> loadModel(float* vertices,
                                                                                                 size_t verticeLength,
                                                                                                 std::int32_t* indices,
                                                                                                 size_t indiceLength,
                                                                                                 float* textureCoords,
                                                                                                 size_t textureLength,
                                                                                                 float* normals,
                                                                                                 size_t normalsLength,
                                                                                                 std::string mat,
                                                                                                 glm::vec3 min,
                                                                                                 glm::vec3 max);
  std::tuple<std::unique_ptr<Model>, std::unique_ptr<Material>, std::unique_ptr<AABB>> loadModel(std::string path);
  void unloadModel(Model* model, Material* material);
  GLuint loadTexture(std::string textureFile);
  void cleanup();

private:
  std::vector<GLuint> _vaos;
  std::vector<GLuint> _vbos;
  std::vector<GLuint> _textures;
  WestLogger* _logger;


  std::tuple<std::unique_ptr<Model>, std::unique_ptr<Material>, std::unique_ptr<AABB>> loadOBJModel(FILE* file);
  GLuint createVAO();
  void storeIndicesBuffer(std::int32_t* data, size_t dataLength);
  void storeDataInAttribList(std::int32_t attribNo, std::int32_t vertexCount, float* data, size_t dataLength);
  void unbind();
  std::vector<float> generatePlanarUV(const std::vector<float>& vertices);
  std::unique_ptr<Material> generateMaterialFromMTL(const std::string& path);
};
