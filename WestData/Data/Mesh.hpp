#pragma once


#include "Texture.hpp"
#include "Vertex.hpp"

#include <cstdint>
#include <vector>

class Mesh
{
public:
  std::vector<Vertex> vertices;
  std::vector<std::uint32_t> indices;
  std::vector<Texture> textures;
  glm::vec3 baseColor;
  glm::vec3 emissiveColor;
  glm::vec3 diffuseColor;

  Mesh(std::string guid,
       std::vector<Vertex>& vertices,
       std::vector<std::uint32_t>& indices,
       std::vector<Texture>& textures)
  {
    this->_guid         = guid;
    this->vertices      = vertices;
    this->indices       = indices;
    this->textures      = textures;
    this->baseColor     = glm::vec3(1.0, 0.0, 0.0);
    this->emissiveColor = glm::vec3(0.0, 0.0, 0.0);
  };


  const std::string& getGuid()
  {
    return _guid;
  };

private:
  std::string _guid;
};
