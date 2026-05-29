#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>

class Model
{
public:
  Model();
  ~Model();

  std::size_t getGuid();
  std::vector<Mesh>& getMeshes();

  void addMesh(const Mesh& m)
  {
    _meshes.push_back(m);
  }

  void setGuid(const std::string guid)
  {
    _guid = guid;
  };

private:
  std::string _guid;
  std::vector<Mesh> _meshes;
};
