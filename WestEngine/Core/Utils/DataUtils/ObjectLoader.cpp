#include "../../../CoreHeaders/Utils/DataUtils/ObjectLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include <format>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <PathUtils.h>
#include <stb_image.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <limits.h>
#endif

std::tuple<Model*, Material*> ObjectLoader::loadModel(float* vertices,
                                                      size_t verticesLength,
                                                      std::int32_t* indices,
                                                      size_t indicesLength,
                                                      float* texture,
                                                      size_t textureLength,
                                                      float* normals,
                                                      size_t normalsLength,
                                                      std::string mat)
{
  assert(vertices != nullptr && verticesLength > 0 && indices != nullptr && indicesLength > 0 && _logger != nullptr);

  GLuint id = createVAO();
  storeIndicesBuffer(indices, indicesLength);
  storeDataInAttribList(0, 3, vertices, verticesLength);
  if (textureLength > 0)
  {
    storeDataInAttribList(1, 2, texture, textureLength);
  }
  if (normalsLength > 0)
  {
    storeDataInAttribList(2, 3, normals, normalsLength);
  }

  unbind();

#ifdef DEBUG
  _logger->log(Level::Info, "---Loaded Model, stored Data in vbo and vao\n");
#endif
  Material* material = nullptr;
  if (mat.size() == 0)
  {
#ifdef DEBUG
    _logger->log(Level::Info, "---Falling back to Debug-Material\n");
#endif
    material               = new Material();
    material->diffuseColor = glm::vec3(1, 0, 0);
  }
  else
  {
    material = generateMaterialFromMTL(mat);
  }

  Model* m       = new Model();
  m->id          = id;
  m->vertexCount = indicesLength / sizeof(std::int32_t);
  assert(material != nullptr);
  return std::make_tuple(m, material);
}

std::tuple<Model*, Material*> ObjectLoader::loadModel(std::string path)
{
  FILE* file = PathUtils::openFile(path, true);
  if (file == nullptr)
  {
    return std::make_tuple(nullptr, nullptr);
  }
  return loadOBJModel(file);
}

std::tuple<Model*, Material*> ObjectLoader::loadOBJModel(FILE* file)
{
  std::vector<float> vertices;
  std::vector<std::int32_t> indices;
  std::vector<float> textures;
  std::vector<float> normals;
  std::vector<float> mappedNormals;
  std::vector<float> mappedTextures;
  std::string material;

  char line[256];
  while (fgets(line, sizeof(line), file))
  {
    if (strncmp(line, "mtllib ", 7) == 0)
    {
      char mat[16];
      sscanf(line + 7, "%s", mat);
      material = std::string(std::format("/assets/Models/{}", mat));
    }
    else if (strncmp(line, "v ", 2) == 0)
    {
      float x, y, z;
      sscanf(line + 2, "%f %f %f", &x, &y, &z);
      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(z);
    }
    else if (strncmp(line, "vt ", 3) == 0)
    {
      float u, v;
      sscanf(line + 3, "%f %f", &u, &v);
      textures.push_back(u);
      textures.push_back(v);
    }
    else if (strncmp(line, "vn ", 3) == 0)
    {
      float nx, ny, nz;
      sscanf(line + 3, "%f %f %f", &nx, &ny, &nz);
      normals.push_back(nx);
      normals.push_back(ny);
      normals.push_back(nz);
    }
    else if (strncmp(line, "f ", 2) == 0)
    {
      std::int32_t vertexIndex[4], textureIndex[4] = {0}, normalIndex[4] = {0};
      int faceVertices = 0;
      int format       = 0;  // 0 = unknown, 1 = v/vt/vn, 2 = v/vt, 3 = v//vn, 4 = v only

      // Try format: v/vt/vn (full format with 4 vertices - quad)
      int matches = sscanf(line + 2,
                           "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d",
                           &vertexIndex[0],
                           &textureIndex[0],
                           &normalIndex[0],
                           &vertexIndex[1],
                           &textureIndex[1],
                           &normalIndex[1],
                           &vertexIndex[2],
                           &textureIndex[2],
                           &normalIndex[2],
                           &vertexIndex[3],
                           &textureIndex[3],
                           &normalIndex[3]);
      if (matches == 12)
      {
        faceVertices = 4;
        format       = 1;
      }

      // Try format: v/vt/vn (full format with 3 vertices - triangle)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2,
                         "%d/%d/%d %d/%d/%d %d/%d/%d",
                         &vertexIndex[0],
                         &textureIndex[0],
                         &normalIndex[0],
                         &vertexIndex[1],
                         &textureIndex[1],
                         &normalIndex[1],
                         &vertexIndex[2],
                         &textureIndex[2],
                         &normalIndex[2]);
        if (matches == 9)
        {
          faceVertices = 3;
          format       = 1;
        }
      }

      // Try format: v//vn (no texture, 4 vertices - quad)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2,
                         "%d//%d %d//%d %d//%d %d//%d",
                         &vertexIndex[0],
                         &normalIndex[0],
                         &vertexIndex[1],
                         &normalIndex[1],
                         &vertexIndex[2],
                         &normalIndex[2],
                         &vertexIndex[3],
                         &normalIndex[3]);
        if (matches == 8)
        {
          faceVertices = 4;
          format       = 3;
        }
      }

      // Try format: v//vn (no texture, 3 vertices - triangle)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2,
                         "%d//%d %d//%d %d//%d",
                         &vertexIndex[0],
                         &normalIndex[0],
                         &vertexIndex[1],
                         &normalIndex[1],
                         &vertexIndex[2],
                         &normalIndex[2]);
        if (matches == 6)
        {
          faceVertices = 3;
          format       = 3;
        }
      }

      // Try format: v/vt (no normals, 4 vertices - quad)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2,
                         "%d/%d %d/%d %d/%d %d/%d",
                         &vertexIndex[0],
                         &textureIndex[0],
                         &vertexIndex[1],
                         &textureIndex[1],
                         &vertexIndex[2],
                         &textureIndex[2],
                         &vertexIndex[3],
                         &textureIndex[3]);
        if (matches == 8)
        {
          faceVertices = 4;
          format       = 2;
        }
      }

      // Try format: v/vt (no normals, 3 vertices - triangle)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2,
                         "%d/%d %d/%d %d/%d",
                         &vertexIndex[0],
                         &textureIndex[0],
                         &vertexIndex[1],
                         &textureIndex[1],
                         &vertexIndex[2],
                         &textureIndex[2]);
        if (matches == 6)
        {
          faceVertices = 3;
          format       = 2;
        }
      }

      // Try format: v only (4 vertices - quad)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2, "%d %d %d %d", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2], &vertexIndex[3]);
        if (matches == 4)
        {
          faceVertices = 4;
          format       = 4;
        }
      }

      // Try format: v only (3 vertices - triangle)
      if (faceVertices == 0)
      {
        matches = sscanf(line + 2, "%d %d %d", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
        if (matches == 3)
        {
          faceVertices = 3;
          format       = 4;
        }
      }

      if (faceVertices >= 3)
      {
        // Helper lambda to process a single triangle
        auto processTriangle = [&](int i0, int i1, int i2)
        {
          indices.push_back(vertexIndex[i0] - 1);
          indices.push_back(vertexIndex[i1] - 1);
          indices.push_back(vertexIndex[i2] - 1);

          if (format == 1 || format == 2)  // Has texture coords
          {
            int triIndices[3] = {i0, i1, i2};
            for (int i = 0; i < 3; i++)
            {
              int texIndex = textureIndex[triIndices[i]] - 1;
              if (texIndex >= 0 && static_cast<size_t>(texIndex) < textures.size() / 2)
              {
                mappedTextures.push_back(textures[texIndex * 2]);
                mappedTextures.push_back(textures[texIndex * 2 + 1]);
              }
            }
          }

          if (format == 1 || format == 3)  // Has normals
          {
            int triIndices[3] = {i0, i1, i2};
            for (int i = 0; i < 3; i++)
            {
              int normIndex = normalIndex[triIndices[i]] - 1;
              if (normIndex >= 0 && static_cast<size_t>(normIndex) < normals.size() / 3)
              {
                mappedNormals.push_back(normals[normIndex * 3]);
                mappedNormals.push_back(normals[normIndex * 3 + 1]);
                mappedNormals.push_back(normals[normIndex * 3 + 2]);
              }
            }
          }
        };

        // Process first triangle (vertices 0, 1, 2)
        processTriangle(0, 1, 2);

        // If quad, process second triangle (vertices 0, 2, 3)
        if (faceVertices == 4)
        {
          processTriangle(0, 2, 3);
        }
      }
      else
      {
        _logger->log(Level::Error, "---Error parsing OBJ face data!");
        return std::make_tuple(nullptr, nullptr);
      }
    }
  }

  fclose(file);
  float* textureArray = nullptr;
  if (mappedTextures.empty() && !vertices.empty())
  {
    mappedTextures = generatePlanarUV(vertices);
    textureArray   = mappedTextures.data();
  }
  else
  {
    textureArray = mappedTextures.data();
  }
  assert(textureArray != nullptr);

  float* vertexArray       = vertices.empty() ? nullptr : vertices.data();
  std::int32_t* indexArray = indices.empty() ? nullptr : indices.data();
  float* normalArray       = mappedNormals.empty() ? nullptr : mappedNormals.data();

  return loadModel(vertexArray,
                   vertices.size() * sizeof(float),
                   indexArray,
                   indices.size() * sizeof(std::int32_t),
                   textureArray,
                   mappedTextures.size() * sizeof(float),
                   normalArray,
                   normals.size() * sizeof(float),
                   material);
}

Material* ObjectLoader::generateMaterialFromMTL(const std::string& path)
{
  FILE* file       = PathUtils::openFile(path, true);
  Material* result = new Material();
  if (file == nullptr)
  {
    return result;
  }
  char line[256];
  while (fgets(line, sizeof(line), file))
  {
    if (strncmp(line, "Kd ", 3) == 0)
    {
      float u, v, z;
      sscanf(line + 3, "%f %f %f", &u, &v, &z);
      result->diffuseColor = glm::vec3(u, v, z);
    }
  }


  return result;
}

GLuint ObjectLoader::loadTexture(std::string textureFile)
{
  assert(_logger != nullptr);
  std::int32_t width, height, numComponents;

  std::string filePath = PathUtils::resolve("/" + textureFile);
#ifdef DEBUG
  _logger->log(Level::Info, std::format("---Loading Texture: {}\n", filePath));
#endif
  unsigned char* imgData = stbi_load(filePath.c_str(), &width, &height, &numComponents, 0);
  if (imgData == NULL)
  {
    _logger->log(
      Level::Error,
      std::format("---No Imagedata loaded for texture: {} - STBI Error: {}\n", filePath, stbi_failure_reason()));
    return -1;
  }

  GLuint id;
  glGenTextures(1, &id);
  _textures.push_back(id);
  glBindTexture(GL_TEXTURE_2D, id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imgData);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(imgData);

#ifdef DEBUG
  _logger->log(Level::Info, "---Loaded Texture, stored Data for Model\n");
#endif
  return id;
}

GLuint ObjectLoader::createVAO()
{
  GLuint vao;
  glGenVertexArrays(1, &vao);
  _vaos.push_back(vao);
  glBindVertexArray(vao);
  return vao;
}

void ObjectLoader::storeIndicesBuffer(std::int32_t* data, size_t dataLength)
{
  assert(data != nullptr);
  GLuint vbo;
  glGenBuffers(1, &vbo);
  _vbos.push_back(vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataLength, data, GL_STATIC_DRAW);
}

void ObjectLoader::storeDataInAttribList(std::int32_t attribNo,
                                         std::int32_t vertexCount,
                                         float* data,
                                         size_t dataLength)
{
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

void ObjectLoader::unbind()
{
  glBindVertexArray(0);
}

void ObjectLoader::unloadModel(Model* model, Material* material)
{
  assert(_logger != nullptr);
  if (!model)
  {
    return;
  }

  GLuint vaoId     = model->id;
  GLuint textureId = 0;
  if (material->diffuseTexture != nullptr)
  {
    textureId = material->diffuseTexture->id;
  }

  auto vaoIt   = std::find(_vaos.begin(), _vaos.end(), vaoId);
  GLuint index = distance(_vaos.begin(), vaoIt);

  if (vaoIt != _vaos.end())
  {
    glDeleteVertexArrays(1, &vaoId);
    _vaos.erase(vaoIt);
  }

  if (textureId > 0)
  {
    GLuint bufferIds[3] = {_vbos[index], _vbos[index + 1], _vbos[index + 2]};
    glDeleteBuffers(3, bufferIds);
    _vbos.erase(_vbos.begin() + index, _vbos.begin() + index + 3);
  }
  else
  {
    GLuint bufferIds[2] = {_vbos[index], _vbos[index + 1]};
    glDeleteBuffers(2, bufferIds);
    _vbos.erase(_vbos.begin() + index, _vbos.begin() + index + 2);
  }

  auto textureIt = std::find(_textures.begin(), _textures.end(), textureId);
  if (textureIt != _textures.end())
  {
    glDeleteTextures(1, &textureId);
    _textures.erase(textureIt);
  }

#ifdef DEBUG
  _logger->log(Level::Info, "--- Deleted Vertex Array, Buffer and Textures from gl\n");
#endif
}

std::vector<float> ObjectLoader::generatePlanarUV(const std::vector<float>& vertices)
{
  std::vector<float> uvs;
  float minX = vertices[0], maxX = vertices[0];
  float minZ = vertices[2], maxZ = vertices[2];

  for (size_t i = 0; i < vertices.size(); i += 3)
  {
    float x = vertices[i];
    float z = vertices[i + 2];

    minX = std::min(minX, x);
    maxX = std::max(maxX, x);
    minZ = std::min(minZ, z);
    maxZ = std::max(maxZ, z);
  }

  float rangeX = maxX - minX;
  float rangeZ = maxZ - minZ;

  for (size_t i = 0; i < vertices.size(); i += 3)
  {
    float x = vertices[i];
    float z = vertices[i + 2];

    float u = (rangeX > 0) ? (x - minX) / rangeX : 0.5f;
    float v = (rangeZ > 0) ? (z - minZ) / rangeZ : 0.5f;

    uvs.push_back(u);
    uvs.push_back(v);
  }

  _logger->log(Level::Info, "--- No UV specified for given object, creating default ones.\n");

  return uvs;
}

void ObjectLoader::cleanup()
{
  if (_vaos.size() > 0)
  {
    glDeleteVertexArrays(_vaos.size(), _vaos.data());
  }
  if (_vbos.size() > 0)
  {
    glDeleteBuffers(_vbos.size(), _vbos.data());
  }
  if (_textures.size() > 0)
  {
    glDeleteBuffers(_textures.size(), _textures.data());
  }
}
