#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>

class Model
{
public:
  Model(const std::string& path);
  ~Model();

  std::size_t getHash() const;
  const std::vector<Mesh>& getMeshes();

private:
  std::size_t _model;
  std::vector<Mesh> _meshes;
};
