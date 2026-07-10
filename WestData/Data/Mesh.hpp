#pragma once


#include "AABB.hpp"
#include "Material.hpp"
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
  Material material;
  AABB aabb;

  Mesh(std::string guid,
       std::vector<Vertex>& vertices,
       std::vector<std::uint32_t>& indices,
       std::vector<Texture>& textures,
       AABB aabb)
  {
    this->_guid    = guid;
    this->vertices = vertices;
    this->indices  = indices;
    this->textures = textures;
    this->aabb     = aabb;
    this->material = Material{};
  };

  const std::string& getGuid()
  {
    return _guid;
  };

private:
  std::string _guid;
};
