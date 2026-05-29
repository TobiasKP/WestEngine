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

  Mesh(std::string guid,
       std::vector<Vertex>& vertices,
       std::vector<std::uint32_t>& indices,
       std::vector<Texture>& textures);

  std::string getGuid();

private:
  std::string _guid;
};
