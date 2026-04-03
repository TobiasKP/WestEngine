#pragma once

#include "../Data/Mesh.hpp"

class OpenGLDataManager
{
public:
  OpenGLDataManager();
  ~OpenGLDataManager();

  void setupMesh(Mesh& m);
  void degradeMesh(Mesh& m);

private:
};
