#include "ShaderController.hpp"

#include <PathUtils.h>
#include <format>

GLuint ShaderController::createShader(const std::string shaderFile, std::int32_t shaderType, GLuint programId)
{
  GLuint shaderId = glCreateShader(shaderType);
  if (shaderId == 0)
  {
    _logger->log(Level::Error, std::format("Error creating shader. Type: {}.\n", shaderType));
    return -1;
  }

#ifdef DEBUG
  _logger->log(Level::Info, std::format("|><| Created Shader: {} for File: {}.\n", shaderId, shaderFile));
#endif

  const GLchar* source = readShaderSource(shaderFile);
  if (source == NULL)
  {
    _logger->log(Level::Error, "Failed to read shader source from file\n");
    return -1;
  }
  GLchar errorLog[2048] = {};
  GLint size = 0, status = 0;
  glShaderSource(shaderId, 1, &source, NULL);
  glCompileShader(shaderId);

  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
  if (status == 0)
  {
    glGetShaderInfoLog(shaderId, 2048, &size, errorLog);
    _logger->log(Level::Error, std::format("Error compiling shader. Type: {}, Info: {}\n", shaderType, errorLog));
    return -1;
  }

  glAttachShader(programId, shaderId);
  return shaderId;
}

void ShaderController::link(GLuint programId, GLuint vertexId, GLuint fragmentId)
{
  glLinkProgram(programId);
  GLint size = 0, status = 0;
  GLchar errorLog[1024] = {};
  glGetProgramiv(programId, GL_LINK_STATUS, &status);
  if (status == 0)
  {
    glGetProgramInfoLog(programId, 1024, &size, errorLog);
    _logger->log(Level::Error, std::format("Error linking program. Info: {}\n", errorLog));
  }

  if (vertexId != 0)
  {
    glDetachShader(programId, vertexId);
  }

  if (fragmentId != 0)
  {
    glDetachShader(programId, fragmentId);
  }

  glValidateProgram(programId);
  glGetProgramiv(programId, GL_VALIDATE_STATUS, &status);
  if (status == 0)
  {
    _logger->log(Level::Error, std::format("Error validating program. Info: {}\n", errorLog));
  }
}

GLchar* ShaderController::readShaderSource(const std::string shaderFile)
{
  FILE* file = PathUtils::openFile(shaderFile, true);
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  rewind(file);
  char* source = (char*)malloc((fileSize + 1) * sizeof(char));
  if (source == NULL)
  {
    _logger->log(Level::Error, "Error allocating memory.\n");
    fclose(file);
    return nullptr;
  }

  size_t bytesRead = fread(source, sizeof(char), fileSize, file);
  if (bytesRead != fileSize)
  {
    _logger->log(Level::Error, "Could not read the Entire file.\n");
    fclose(file);
    free(source);
    return nullptr;
  }

  source[fileSize] = '\0';
  fclose(file);
  return source;
}
