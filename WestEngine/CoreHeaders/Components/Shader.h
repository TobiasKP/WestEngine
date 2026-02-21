#pragma once

#include "../Interfaces/IComponent.h"

#include <CoreConstants.hpp>
#include <GL/glew.h>
#include <string>

struct Shader : public IComponent
{
  bool initialized = false;
  GLuint programId, shadergroup;
  std::string vertexShaderFile = CoreConstants::UNDEFINED_STRING.data();
  std::string fragShaderFile   = CoreConstants::UNDEFINED_STRING.data();
};
