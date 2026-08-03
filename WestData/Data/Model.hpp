#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>

class Model
{
public:
  Model() {};
  ~Model() {};

  const std::string& getGuid() const
  {
    return _guid;
  };

  const std::vector<Mesh>& getMeshes() const
  {
    return _meshes;
  };

  std::vector<Mesh>& getMeshes()
  {
    return _meshes;
  };

  const std::string& getName() const
  {
    return _name;
  }

  void addMesh(const Mesh& m)
  {
    _meshes.push_back(m);
  }

  void setGuid(const std::string& guid)
  {
    _guid = guid;
  };

  void setName(const std::string& name)
  {
    _name = name;
  };

private:
  std::string _guid;
  std::string _name;
  std::vector<Mesh> _meshes;
};
