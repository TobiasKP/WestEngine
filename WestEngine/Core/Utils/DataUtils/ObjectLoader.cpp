#include "../../../CoreHeaders/Utils/DataUtils/ObjectLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>
#include <fcntl.h>
#include <format>
#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
<include> direct.h
#define getcwd _getcwd
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif

    Model *
    ObjectLoader::loadModel(float *vertices, size_t verticesLength,
                            std::int32_t *indices, size_t indicesLength,
                            float *texture, size_t textureLength,
                            float *normals, size_t normalsLength) {
  assert(vertices != nullptr && verticesLength > 0 && indices != nullptr &&
         indicesLength > 0 && _logger != nullptr);

  GLuint id = createVAO();
  storeIndicesBuffer(indices, indicesLength);
  storeDataInAttribList(0, 3, vertices, verticesLength);
  if (textureLength > 0)
    storeDataInAttribList(1, 2, texture, textureLength);
  if (normalsLength > 0)
    storeDataInAttribList(2, 3, normals, normalsLength);

  unbind();

#ifdef DEBUG
  _logger->log(Level::Info, "---Loaded Model, stored Data in vbo and vao\n");
#endif

  Model *m = new Model();
  m->id = id;
  m->vertexCount = indicesLength / sizeof(std::int32_t);
  return m;
}

Model *ObjectLoader::loadModel(const char *path) {

  char cwd[PATH_MAX];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL)
    return nullptr;

  std::int32_t fd;
  FILE *file;
  snprintf(filePath, sizeof(filePath), "%s%s%s", cwd, "/", path);
  if ((fd = open(filePath, O_RDONLY)) == -1) {
    _logger->log(Level::Error,
                 std::format("---Error opening File!\n Path: {}\n", filePath));
    // TODO return drawdebug Cube
    return nullptr;
  }

  if ((file = fdopen(fd, "r")) == NULL) {
    _logger->log(Level::Error, "---Error opening File!\n");
    return nullptr;
  }

  return loadOBJModel(file);
}

Model *ObjectLoader::loadOBJModel(FILE *file) {
  std::vector<float> vertices;
  std::vector<std::int32_t> indices;
  std::vector<float> textures;
  std::vector<float> normals;
  std::vector<float> mappedNormals;
  std::vector<float> mappedTextures;

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "v ", 2) == 0) {
      float x, y, z;
      sscanf(line + 2, "%f %f %f", &x, &y, &z);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    } else if (strncmp(line, "vt ", 3) == 0) {
      float u, v;
      sscanf(line + 3, "%f %f", &u, &v);
      textures.push_back(u);
      textures.push_back(v);
    } else if (strncmp(line, "vn ", 3) == 0) {
      float nx, ny, nz;
      sscanf(line + 3, "%f %f %f", &nx, &ny, &nz);
      normals.push_back(nx);
      normals.push_back(ny);
      normals.push_back(nz);
    } else if (strncmp(line, "f ", 2) == 0) {
      std::int32_t vertexIndex[3], textureIndex[3] = {0}, normalIndex[3] = {0};
      int matches = sscanf(line + 2, "%d/%d/%d %d/%d/%d %d/%d/%d",
                           &vertexIndex[0], &textureIndex[0], &normalIndex[0],
                           &vertexIndex[1], &textureIndex[1], &normalIndex[1],
                           &vertexIndex[2], &textureIndex[2], &normalIndex[2]);

      if (matches != 9) {
        matches = sscanf(line + 2, "%d/%d %d/%d %d/%d", &vertexIndex[0],
                         &textureIndex[0], &vertexIndex[1], &textureIndex[1],
                         &vertexIndex[2], &textureIndex[2]);
      }

      if (matches != 6) {
        matches = sscanf(line + 2, "%d %d %d", &vertexIndex[0], &vertexIndex[1],
                         &vertexIndex[2]);
      }

      if (matches >= 3) {
        indices.push_back(vertexIndex[0] - 1);
        indices.push_back(vertexIndex[1] - 1);
        indices.push_back(vertexIndex[2] - 1);

        if (matches >= 6) {
          for (int i = 0; i < 3; i++) {
            int texIndex = textureIndex[i] - 1;
            if (texIndex >= 0 && texIndex < textures.size() / 2) {
              mappedTextures.push_back(textures[texIndex * 2]);
              mappedTextures.push_back(textures[texIndex * 2 + 1]);
            }
          }
        }

        if (matches == 9) {
          for (int i = 0; i < 3; i++) {
            int normIndex = normalIndex[i] - 1;
            if (normIndex >= 0 && normIndex < normals.size() / 3) {
              mappedNormals.push_back(normals[normIndex * 3]);
              mappedNormals.push_back(normals[normIndex * 3 + 1]);
              mappedNormals.push_back(normals[normIndex * 3 + 2]);
            }
          }
        }
      } else {
        _logger->log(Level::Error, "---Error parsing OBJ face data!");
        return nullptr;
      }
    }
  }

  fclose(file);
  float *textureArray = nullptr;
  if (mappedTextures.empty() && !vertices.empty()) {
    mappedTextures = generatePlanarUV(vertices);
    textureArray = mappedTextures.data();
  } else {
    textureArray = mappedTextures.data();
  }
  assert(textureArray != nullptr);

  float *vertexArray = vertices.empty() ? nullptr : vertices.data();
  std::int32_t *indexArray = indices.empty() ? nullptr : indices.data();
  float *normalArray = mappedNormals.empty() ? nullptr : mappedNormals.data();

  return loadModel(vertexArray, vertices.size() * sizeof(float), indexArray,
                   indices.size() * sizeof(std::int32_t), textureArray,
                   mappedTextures.size() * sizeof(float), normalArray,
                   normals.size() * sizeof(float));
}

GLuint ObjectLoader::loadTexture(const char *textureFile) {
  assert(_logger != nullptr);
  std::int32_t width, height, numComponents;

  char cwd[128];
  char filePath[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    _logger->log(Level::Error, "---Error getting current working directory!\n");
    return -1;
  }

  snprintf(filePath, sizeof(filePath), "%s%s%s", cwd, "/", textureFile);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("---Loading Texture: {}\n", filePath));
#endif
  unsigned char *imgData =
      stbi_load(filePath, &width, &height, &numComponents, 0);
  if (imgData == NULL) {
    _logger->log(
        Level::Error,
        std::format("---No Imagedata loaded for texture: {} - STBI Error: {}\n",
                    filePath, stbi_failure_reason()));
    return -1;
  }

  GLuint id;
  glGenTextures(1, &id);
  _textures.push_back(id);
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imgData);

#ifdef DEBUG
  _logger->log(Level::Info, "---Loaded Texture, stored Data for Model\n");
#endif
  return id;
}

GLuint ObjectLoader::createVAO() {
  GLuint vao;
  glGenVertexArrays(1, &vao);
  _vaos.push_back(vao);
  glBindVertexArray(vao);
  return vao;
}

void ObjectLoader::storeIndicesBuffer(std::int32_t *data, size_t dataLength) {
  assert(data != nullptr);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  _vbos.push_back(vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);
}

void ObjectLoader::storeDataInAttribList(std::int32_t attribNo,
                                         std::int32_t vertexCount, float *data,
                                         size_t dataLength) {
  assert(data != nullptr && attribNo >= 0 && vertexCount > 0);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  _vbos.push_back(vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);
  glVertexAttribPointer(attribNo, vertexCount, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(attribNo);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ObjectLoader::unbind() { glBindVertexArray(0); }

void ObjectLoader::unloadModel(Model *model) {
  assert(_logger != nullptr);
  if (!model)
    return;

  GLuint vaoId = model->id;
  GLuint textureId = 0;
  if (model->texture != nullptr)
    textureId = model->texture->id;

  auto vaoIt = std::find(_vaos.begin(), _vaos.end(), vaoId);
  GLuint index = distance(_vaos.begin(), vaoIt);

  if (vaoIt != _vaos.end()) {
    glDeleteVertexArrays(1, &vaoId);
    _vaos.erase(vaoIt);
  }

  if (textureId > 0) {
    GLuint bufferIds[3] = {_vbos[index], _vbos[index + 1], _vbos[index + 2]};
    glDeleteBuffers(3, bufferIds);
    _vbos.erase(_vbos.begin() + index, _vbos.begin() + index + 3);
  } else {
    GLuint bufferIds[2] = {_vbos[index], _vbos[index + 1]};
    glDeleteBuffers(2, bufferIds);
    _vbos.erase(_vbos.begin() + index, _vbos.begin() + index + 2);
  }

  auto textureIt = std::find(_textures.begin(), _textures.end(), textureId);
  if (textureIt != _textures.end()) {
    glDeleteTextures(1, &textureId);
    _textures.erase(textureIt);
  }

#ifdef DEBUG
  _logger->log(Level::Info,
               "--- Deleted Vertex Array, Buffer and Textures from gl\n");
#endif
}

std::vector<float>
ObjectLoader::generatePlanarUV(const std::vector<float> &vertices) {
  std::vector<float> uvs;
  float minX = vertices[0], maxX = vertices[0];
  float minZ = vertices[2], maxZ = vertices[2];

  for (size_t i = 0; i < vertices.size(); i += 3) {
    float x = vertices[i];
    float z = vertices[i + 2];

    minX = std::min(minX, x);
    maxX = std::max(maxX, x);
    minZ = std::min(minZ, z);
    maxZ = std::max(maxZ, z);
  }

  float rangeX = maxX - minX;
  float rangeZ = maxZ - minZ;

  for (size_t i = 0; i < vertices.size(); i += 3) {
    float x = vertices[i];
    float z = vertices[i + 2];

    float u = (rangeX > 0) ? (x - minX) / rangeX : 0.5f;
    float v = (rangeZ > 0) ? (z - minZ) / rangeZ : 0.5f;

    uvs.push_back(u);
    uvs.push_back(v);
  }

  _logger->log(
      Level::Info,
      "--- No UV specified for given object, creating default ones.\n");

  return uvs;
}

void ObjectLoader::cleanup() {
  if (_vaos.size() > 0)
    glDeleteVertexArrays(_vaos.size(), _vaos.data());
  if (_vbos.size() > 0)
    glDeleteBuffers(_vbos.size(), _vbos.data());
  if (_textures.size() > 0)
    glDeleteBuffers(_textures.size(), _textures.data());
}
