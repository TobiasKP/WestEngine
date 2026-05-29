#pragma once

#include "Mesh.hpp"

#include <string>
#include <vector>

class Model
{
public:
  Model();
  ~Model();

  std::string& getGuid()
  {
    return _guid;
  };

  std::vector<Mesh>& getMeshes()
  {
    return _meshes;
  };

  std::string& getName()
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
