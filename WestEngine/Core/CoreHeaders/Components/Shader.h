#pragma once

#include <GL/glew.h>

#include "../Interfaces/IComponent.h"

struct Shader : public IComponent {
  bool initialized = false;
  GLuint programId, shadergroup;
  char *vertexShaderFile;
  char *fragShaderFile;
};
