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

  std::tuple<Model*, Material*> loadModel(float* vertices,
                                          size_t verticeLength,
                                          std::int32_t* indices,
                                          size_t indiceLength,
                                          float* textureCoords,
                                          size_t textureLength,
                                          float* normals,
                                          size_t normalsLength,
                                          std::string mat);
  std::tuple<Model*, Material*> loadModel(std::string path);
  void unloadModel(Model* model, Material* material);
  GLuint loadTexture(std::string textureFile);
  void cleanup();

private:
  std::vector<GLuint> _vaos;
  std::vector<GLuint> _vbos;
  std::vector<GLuint> _textures;
  WestLogger* _logger;
  
 
  std::tuple<Model*, Material*> loadOBJModel(FILE* file);
  GLuint createVAO();
  void storeIndicesBuffer(std::int32_t* data, size_t dataLength);
  void storeDataInAttribList(std::int32_t attribNo, std::int32_t vertexCount, float* data, size_t dataLength);
  void unbind();
  std::vector<float> generatePlanarUV(const std::vector<float>& vertices);
  Material* generateMaterialFromMTL(const std::string& path);
};
