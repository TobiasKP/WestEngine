#include "../../../CoreHeaders/Utils/DataUtils/UniformUtils.h"

#include "glm/gtc/type_ptr.hpp"

#include <stdio.h>
#include <WestLogger.h>

GLuint UniformUtils::createUniform(const char* name, GLuint programId)
{
  GLint location = glGetUniformLocation(programId, name);
  if (location == -1)
  {
    char err[128] = "Could not create Uniform: ";
    strcat(err, name);
    strcat(err, "\n");
    WestLogger::getLoggerInstance().log(Level::Error, err);
  }
  return location;
}

void UniformUtils::setUniform(GLuint location, const std::vector<uint32_t>& value)
{
  glUniform1uiv(location, value.size(), value.data());
}

void UniformUtils::setUniform(GLuint location, std::int32_t value)
{
  glUniform1i(location, value);
}

void UniformUtils::setUniform(GLuint location, std::uint32_t value)
{
  glUniform1ui(location, value);
}

void UniformUtils::setUniform(GLuint location, bool value)
{
  glUniform1f(location, value);
}

void UniformUtils::setUniform(GLuint location, float value)
{
  glUniform1f(location, value);
}

void UniformUtils::setUniform(GLuint location, glm::mat4 value)
{
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void UniformUtils::setUniform(GLuint location, glm::vec3 value)
{
  glUniform3fv(location, 1, glm::value_ptr(value));
}

void UniformUtils::setUniform(GLuint location, glm::vec4 value)
{
  glUniform4fv(location, 1, glm::value_ptr(value));
}

void UniformUtils::setUniform(GLuint location, glm::vec2 value)
{
  glUniform2fv(location, 1, glm::value_ptr(value));
}

GLuint UniformUtils::createUniformBufferObject(const char* name, size_t size, GLuint bindingPoint)
{
  GLuint ubo;
  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
  glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, ubo);
  return ubo;
}
