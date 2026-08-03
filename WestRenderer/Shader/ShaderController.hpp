#pragma once

#include <GL/glew.h>
#include <WestLogger.h>

class ShaderController
{
public:
  ShaderController(WestLogger* logger) : _logger(logger) {};
  ~ShaderController() {};

  void link(GLuint programId, GLuint vertexId, GLuint fragmentId);
  GLuint createVertexShader(const std::string file, GLuint programId)
  {
    return createShader(file, GL_VERTEX_SHADER, programId);
  }
  GLuint createFragmentShader(const std::string file, GLuint programId)
  {
    return createShader(file, GL_FRAGMENT_SHADER, programId);
  }

private:
  GLuint createShader(const std::string shaderFile, std::int32_t shaderTyp, GLuint programId);
  GLchar* readShaderSource(const std::string shaderFile);

  WestLogger* _logger;
};
