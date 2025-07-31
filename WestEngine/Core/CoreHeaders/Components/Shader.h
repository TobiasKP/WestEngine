#pragma once

#include <GL/glew.h>
#include <string>

#include "../Interfaces/IComponent.h"
#include "../../Constants/CoreConstants.h"

struct Shader : public IComponent {
  bool initialized = false;
  GLuint programId, shadergroup;
  std::string vertexShaderFile = CoreConstants::UNDEFINED_STRING;
  std::string fragShaderFile = CoreConstants::UNDEFINED_STRING;
};
