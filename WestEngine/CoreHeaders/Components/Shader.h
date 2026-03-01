#pragma once

#include <CoreConstants.hpp>
#include <GL/glew.h>
#include <string>

struct Shader
{
  bool initialized = false;
  GLuint programId, shadergroup;
  std::string vertexShaderFile = CoreConstants::UNDEFINED_STRING.data();
  std::string fragShaderFile   = CoreConstants::UNDEFINED_STRING.data();
};
