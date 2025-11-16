#pragma once

#include "../../Constants/CoreConstants.hpp"
#include "../Interfaces/IComponent.h"

#include <GL/glew.h>
#include <string>

struct Shader : public IComponent
{
  bool initialized = false;
  GLuint programId, shadergroup;
  std::string vertexShaderFile = CoreConstants::UNDEFINED_STRING;
  std::string fragShaderFile   = CoreConstants::UNDEFINED_STRING;
};
