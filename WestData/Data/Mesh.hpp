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

  Mesh(const std::string guid,
       const std::vector<Vertex>& vertices,
       const std::vector<std::uint32_t>& indices,
       const std::vector<Texture>& textures);

  std::string getGuid();

private:
  const std::string _guid;
};
