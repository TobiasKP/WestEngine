#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>

class Model
{
public:
  Model();
  ~Model();

  const std::size_t getGuid(); 
  const std::vector<Mesh>& getMeshes();

private:
  std::string _guid; 
  std::vector<Mesh> _meshes;
};
