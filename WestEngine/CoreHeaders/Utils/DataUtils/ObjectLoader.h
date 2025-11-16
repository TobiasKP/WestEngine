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

  Model* loadModel(float* vertices,
                   size_t verticeLength,
                   std::int32_t* indices,
                   size_t indiceLength,
                   float* textureCoords,
                   size_t textureLength,
                   float* normals,
                   size_t normalsLength);
  Model* loadModel(const char* path);
  void unloadModel(Model* model);
  GLuint loadTexture(const char* textureFile);
  void cleanup();

private:
  std::vector<GLuint> _vaos;
  std::vector<GLuint> _vbos;
  std::vector<GLuint> _textures;
  WestLogger* _logger;

  Model* loadOBJModel(FILE* file);
  GLuint createVAO();
  void storeIndicesBuffer(std::int32_t* data, size_t dataLength);
  void storeDataInAttribList(std::int32_t attribNo, std::int32_t vertexCount, float* data, size_t dataLength);
  void unbind();
  std::vector<float> generatePlanarUV(const std::vector<float>& vertices);
};
